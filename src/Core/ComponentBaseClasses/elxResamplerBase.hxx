#ifndef __elxResamplerBase_hxx
#define __elxResamplerBase_hxx

#include "elxResamplerBase.h"

namespace elastix
{
	using namespace itk;


	/*
	 * ********************* Constructor ****************************
	 */
	
	template <class TElastix>
		ResamplerBase<TElastix>::ResamplerBase()
	{
		/** Create caster and writer.*/
		m_Caster	=	CasterType::New();
		m_Writer	= WriterType::New();
		
	} // end Constructor


	/*
	 * ******************* BeforeRegistrationBase *******************
	 */
	
	template<class TElastix>
		void ResamplerBase<TElastix>
		::BeforeRegistrationBase(void)
	{
		/** Connect the components.*/
		this->SetComponents();
		
		/** Set the size of the image to be produced by the resampler.*/
		
		/** Get a pointer to the fixedImage.*/
		OutputImageType * fixedImage = dynamic_cast<OutputImageType *>(
			m_Elastix->GetFixedImage() );
		
		/** Set the region info to the same values as in the fixedImage.*/
		this->GetAsITKBaseType()->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
		this->GetAsITKBaseType()->SetOutputStartIndex( fixedImage->GetLargestPossibleRegion().GetIndex() );
		this->GetAsITKBaseType()->SetOutputOrigin( fixedImage->GetOrigin() );
		this->GetAsITKBaseType()->SetOutputSpacing( fixedImage->GetSpacing() );
		
		/** Set the DefaultPixelValue (for pixels in the resampled image
		 * that come from outside the original (moving) image.
		 */
		unsigned int defaultPixelValue = 0;
		m_Configuration->ReadParameter( defaultPixelValue, "DefaultPixelValue", 0 );
		
		/** Set the defaultPixelValue in the Superclass.*/
		this->GetAsITKBaseType()->SetDefaultPixelValue( defaultPixelValue );

	} // end BeforeRegistrationBase


	/*
	 * ******************* AfterRegistrationBase ********************
	 */
	
	template<class TElastix>
		void ResamplerBase<TElastix>
		::AfterRegistrationBase(void)
	{
		/** Call WriteToFile().
		 * We call the overwritten WriteToFile in the derived class
		 * (MyStandardResampler), which in turn calls the one of this Base class.
		 */

		/** Apply the final transform, and save the result.*/
		elxout << std::endl << "Applying final transform ..." << std::endl;
		
		this->GetElastix()->GetElxTransformBase()->GetAsITKBaseType()->SetParameters(
			m_Registration->GetAsITKBaseType()->GetLastTransformParameters() );
		
		m_Caster->SetInput( this->GetAsITKBaseType()->GetOutput() );
		m_Writer->SetInput( m_Caster->GetOutput() );
		
		/** Create a name for the final result.*/
		
		std::ostringstream makeFileName( "" );
		makeFileName << m_Configuration->GetCommandLineArgument( "-out" )
			<< "result." << m_Configuration->GetElastixLevel() << ".mhd";
		
		/** Set the filename.*/
		m_Writer->SetFileName( makeFileName.str().c_str() );

		/** Do the writing.*/
		try
		{
			m_Writer->Update();
		}
		catch( itk::ExceptionObject & excp )
		{
			xl::xout["error"] << excp << std::endl;
		}

	} // end AfterRegistrationBase


	/*
	 * *********************** SetComponents ************************
	 */
	
	template <class TElastix>
		void ResamplerBase<TElastix>
		::SetComponents(void)
	{
		/** Set the transform, the interpolator and the inputImage
		 * (which is the moving image).
		 */
		this->GetAsITKBaseType()->SetTransform(		dynamic_cast<TransformType *>(
			m_Elastix->GetTransform() ) );
		
		this->GetAsITKBaseType()->SetInterpolator(		dynamic_cast<InterpolatorType *>(
			m_Elastix->GetResampleInterpolator() ) );
		
		this->GetAsITKBaseType()->SetInput( dynamic_cast<InputImageType *>(
			m_Elastix->GetMovingImage() ) );
		
		// TODO: dit zou bijvoorbeeld ook een gridplaatje kunnen zijn.
		
	} // end SetComponents


	/*
	 * ************************* ReadFromFile ***********************
	 */
	
	template<class TElastix>
		void ResamplerBase<TElastix>
		::ReadFromFile(void)
	{
		/** Connect the components.*/
		this->SetComponents();
		
		/** Get spacing, origin and size of the image to be produced by the resampler.*/
		SpacingType			spacing;
		IndexType				index;
		OriginPointType	origin;
		SizeType				size;
		for ( unsigned int i = 0; i < ImageDimension; i++ )
		{
			/** No default size. Read size from the parameter file. */
			m_Configuration->ReadParameter(	size[ i ], "Size", i );

			/** Default index. Read index from the parameter file. */
			index[ i ] = 0;
			m_Configuration->ReadParameter(	index[ i ], "Index", i );

			/** Default spacing. Read spacing from the parameter file. */
			spacing[ i ] = 1.0;
			m_Configuration->ReadParameter(	spacing[ i ], "Spacing", i );

			/** Default origin. Read origin from the parameter file. */
			origin[ i ] = 0.0;
			m_Configuration->ReadParameter(	origin[ i ], "Origin", i );
		}

		/** Check for image size.*/
		unsigned int sum = 0;
		for ( unsigned int i = 0; i < ImageDimension; i++ )
		{
			if ( size[ i ] == 0 ) sum++;
		}
		if ( sum > 0 )
		{
			xl::xout["error"] << "ERROR: One or more image sizes are 0!" << std::endl;
			//TODO: quit program nicely.
		}
		
		/** Set the region info to the same values as in the fixedImage.*/
		this->GetAsITKBaseType()->SetSize( size );
		this->GetAsITKBaseType()->SetOutputStartIndex( index );
		this->GetAsITKBaseType()->SetOutputOrigin( origin );
		this->GetAsITKBaseType()->SetOutputSpacing( spacing );
		
		/** Set the DefaultPixelValue (for pixels in the resampled image
		 * that come from outside the original (moving) image.
		 */
		unsigned int defaultPixelValue = 0;
		m_Configuration->ReadParameter( defaultPixelValue, "DefaultPixelValue", 0 );
		
		/** Set the defaultPixelValue in the Superclass*/
		this->GetAsITKBaseType()->SetDefaultPixelValue( defaultPixelValue );
		
	} // end ReadFromFile


	/**
	 * ******************* WriteToFile ******************************
	 */

	template <class TElastix>
		void ResamplerBase<TElastix>
		::WriteToFile(void)
	{
		/** Write Resampler specific things.*/
		xout["transpar"] << std::endl << "// Resampler specific" << std::endl;

		/** Write the name of the Resampler.*/
		xout["transpar"] << "(Resampler \""
			<< this->elxGetClassName() << "\")" << std::endl;

		/** Write the DefaultPixelValue.*/
		xout["transpar"] << "(DefaultPixelValue " <<
			this->GetAsITKBaseType()->GetDefaultPixelValue() << ")" << std::endl;

	} // end WriteToFile


} // end namespace elastix


#endif // end #ifndef __elxResamplerBase_hxx

