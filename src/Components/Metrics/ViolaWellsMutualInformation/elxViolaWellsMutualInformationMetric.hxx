#ifndef __elxViolaWellsMutualInformationMetric_CXX__
#define __elxViolaWellsMutualInformationMetric_CXX__

#include "elxViolaWellsMutualInformationMetric.h"

namespace elastix
{
using namespace itk;


	/**
	 * ********************* Constructor ****************************
	 */

	template <class TElastix>
		ViolaWellsMutualInformationMetric<TElastix>
		::ViolaWellsMutualInformationMetric()
	{
		/** Initialize.*/
		m_FixedMaskImageReader = 0;
		m_MovingMaskImageReader = 0;

	} // end Constructor


	/**
	 * ************************ BeforeAll ***************************
	 */
	
	template <class TElastix>
		int ViolaWellsMutualInformationMetric<TElastix>
		::BeforeAll(void)
	{
		/** Declare the return value and initialize it.*/
		int returndummy = 0;

		/** Check Command line options and print them to the logfile.*/
		elxout << "Command line options:" << std::endl;
		std::string check = "";

		/** Check for appearance of "-fMask".*/
		check = m_Configuration->GetCommandLineArgument( "-fMask" );
		if ( check == "" )
		{
			elxout << "-fMask\t\tunspecified, so no fixed mask used" << std::endl;
		}
		else
		{
			elxout << "-fMask\t\t" << check << std::endl;
		}

		/** Check for appearance of "-mMask".*/
		check = "";
		check = m_Configuration->GetCommandLineArgument( "-mMask" );
		if ( check == "" )
		{
			elxout << "-mMask\t\tunspecified, so no moving mask used" << std::endl;
		}
		else
		{
			elxout << "-mMask\t\t" << check << std::endl;
		}

		/** Return a value.*/
		return returndummy;

	} // end BeforeAll


	/**
	 * ******************* Initialize ***********************
	 */

	template <class TElastix>
		void ViolaWellsMutualInformationMetric<TElastix>::
		Initialize(void) throw (ExceptionObject)
	{
		TimerPointer timer = TimerType::New();
		timer->StartTimer();
		this->Superclass1::Initialize();
		timer->StopTimer();
		elxout << "Initialization of ViolaWellsMutualInformationMetric metric took: "
			<< static_cast<long>(timer->GetElapsedClockSec() *1000) << " ms." << std::endl;

	} // end Initialize

	
	/**
	 * ******************* BeforeRegistration ***********************
	 */

	template <class TElastix>
		void ViolaWellsMutualInformationMetric<TElastix>::
		BeforeRegistration(void)
	{		
		/** Read masks if necessary.*/
		
		/** Read fixed mask.*/
		std::string fixedMaskFileName = m_Configuration->
			GetCommandLineArgument( "-fMask" );
		if ( !( fixedMaskFileName.empty() ) )
		{
			m_FixedMaskImageReader	= FixedMaskImageReaderType::New();
			m_FixedMaskImageReader->SetFileName( fixedMaskFileName.c_str() );

			/** Do the reading.*/
			try
			{
				m_FixedMaskImageReader->Update();
			}
			catch( itk::ExceptionObject & excp )
			{
				xl::xout["error"] << excp << std::endl;
			}
			/** Set the fixedmask.*/
			this->SetFixedMask( m_FixedMaskImageReader->GetOutput() );

		} // end if ( fixed mask present )
		
		/** Read moving mask.*/
		std::string movingMaskFileName = m_Configuration->
			GetCommandLineArgument( "-mMask" );
		if ( !( movingMaskFileName.empty() ) )
		{
			m_MovingMaskImageReader	= MovingMaskImageReaderType::New();			
			m_MovingMaskImageReader->SetFileName( movingMaskFileName.c_str() );

			/** Do the reading.*/
			try
			{
				m_MovingMaskImageReader->Update();
			}
			catch( itk::ExceptionObject & excp )
			{
				xl::xout["error"] << excp << std::endl;
			}
			/** Set the movingmask.*/
			this->SetMovingMask( m_MovingMaskImageReader->GetOutput() );

		} // end if ( moving mask present )
		
		/***/
		//TODO: Select another kernel function than the Gaussian

	} // end BeforeRegistration
	

	/**
	 * ***************** BeforeEachResolution ***********************
	 */

	template <class TElastix>
		void ViolaWellsMutualInformationMetric<TElastix>::
		BeforeEachResolution(void)
	{
		//TODO: SecondOrderRegularisationMetric aanpassen.
		//Set alpha, which balances the similarity and deformation energy
		// E_total = (1-alpha)*E_sim + alpha*E_def.
		//	metric->SetAlpha(
		//	config.GetAlpha(level));

		/** Get the current resolution level.*/
		unsigned int level = 
			( m_Registration->GetAsITKBaseType() )->GetCurrentLevel();
		
		/** Set the number of histogram bins and spatial samples.*/
		unsigned int numberOfSpatialSamples = 10000;
		//TODO: guess the default numberOfSpatialSamples from the 
		// imagesize, the numberOfParameters, and the number of bins....

		/** Set the intensity standard deviation of the fixed
		 * and moving images. This defines the kernel bandwidth
		 * used in the joint probability distribution calculation.
		 * Default value is 0.4 which works well for image intensities
		 * normalized to a mean of 0 and standard deviation of 1.0.
		 * Value is clamped to be always greater than zero.
		 */
		double fixedImageStandardDeviation = 0.4;
		double movingImageStandardDeviation = 0.4;
		//TODO: calculate them???
		
		/** Read the parameters from the ParameterFile.*/
		m_Configuration->ReadParameter(
			numberOfSpatialSamples, "NumberOfSpatialSamples", level );
		m_Configuration->ReadParameter(
			fixedImageStandardDeviation, "FixedImageStandardDeviation", level );
		m_Configuration->ReadParameter(
			movingImageStandardDeviation, "MovingImageStandardDeviation", level );
		
		/** Set them.*/
		this->SetNumberOfSpatialSamples( numberOfSpatialSamples );
		this->SetFixedImageStandardDeviation( fixedImageStandardDeviation );
		this->SetMovingImageStandardDeviation( movingImageStandardDeviation );
		
		/** Erode and Set masks if necessary.*/

		/** Read the number of resolutions from the ParameterFile.*/
		unsigned int numberOfResolutions = 3;
		m_Configuration->ReadParameter( numberOfResolutions, "NumberOfResolutions", 0 );
		
		/** Erode and Set the fixed Mask if necessary */
		if ( this->GetFixedMask() )
		{
			/**
			*  If more resolution levels are used, the image is subsampled. Before
			*  subsampling the image is smoothed with a Gaussian filter, with variance
			*  (schedule/2)^2. The 'schedule' depends on the resolution level.
			*  The lowest resolution level has a schedule of 2^(nr_of_levels-1).
			*  The 'radius' of the convolution filter is roughly twice the standard deviation.
			*	 Thus, the parts in the edge with size 'radius' are influenced by the background.
			*/
		
			/** Define the radius.*/
			unsigned long radius = static_cast<unsigned long>(
				ceil( pow( 2.0, static_cast<int>(	numberOfResolutions - level - 1 ) ) ) + 1 );

			/** Erode the mask.*/
			this->SetFixedMask( (m_FixedMaskImageReader->GetOutput())->Erode( radius ) );

		} // end if fixedmask present

		/** Erode and Set the moving Mask if necessary */
		if ( this->GetMovingMask() )
		{
			/**
			*	Same story as before. Now the size the of the eroding element is doubled.
			* This is because the gradient of the moving image is used for calculating
			* the derivative of the metric. 
			*/
			
			/** Define the radius.*/
			unsigned long radius = static_cast<unsigned long>(
				ceil( pow( 2.0, static_cast<int>(	numberOfResolutions - level ) ) ) + 1 );

			/** Erode the mask.*/
			this->SetMovingMask( (m_MovingMaskImageReader->GetOutput())->Erode( radius ) );

		} // end if movingmask present
		
	} // end BeforeEachResolution
	
	
} // end namespace elastix


#endif // end #ifndef __elxViolaWellsMutualInformationMetric_CXX__

