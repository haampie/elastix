#ifndef __itkEulerTransform_H__
#define __itkEulerTransform_H__

#include "itkEuler2DTransform.h"
#include "itkEuler3DTransform.h"

namespace itk
{
	
	
	/**
	 * ********************* class EulerGroup ***********************
	 *
	 * This class only contains a dummy class.
	 */
	
	template< unsigned int Dimension >
		class EulerGroup
	{
	public:
		
		template< class TScalarType >
			class Dummy
		{
		public:
			
			/** Typedef's.*/
			typedef Euler3DTransform< TScalarType >										EulerTransform_tmp;
			typedef typename EulerTransform_tmp::AngleType						AngleType;
			
		}; // end class Dummy
		
	}; // end class EulerGroup
	
	
	/**
	 * ******************** class EulerGroup<2> *********************
	 *
	 * This class only contains a dummy class.
	 */
	
	template<>
		class EulerGroup<2>
	{
	public:
		
		template< class TScalarType >
			class Dummy
		{
		public:
			
			/** Typedef's.*/
			typedef Euler2DTransform< TScalarType >			EulerTransform_tmp;
			typedef int	AngleType;
			
		}; // end class Dummy
		
	}; // end class EulerGroup<2>
	
	
	/**
	 * ******************** class EulerGroup<3> *********************
	 *
	 * This class only contains a dummy class.
	 */
	
	template<>
		class EulerGroup<3>
	{
	public:
		
		template< class TScalarType >
			class Dummy
		{
		public:
			
			/** Typedef's.*/
			typedef Euler3DTransform< TScalarType >										EulerTransform_tmp;
			typedef typename EulerTransform_tmp::AngleType						AngleType;
			
		}; // end class Dummy
		
	}; // end class EulerGroup<3>
	
	
	/**
	 * ***************** class EulerGroupTemplate *******************
	 *
	 * This class
	 */
	
	template< class TScalarType, unsigned int Dimension >
		class EulerGroupTemplate
	{
	public:
					
		typedef EulerGroupTemplate Self;
		typedef TScalarType ScalarType; 			
		itkStaticConstMacro( SpaceDimension, unsigned int, Dimension );					
	
		
		// This declaration of 'Euler' does not work with the GCC compiler
		//		typedef EulerGroup<  itkGetStaticConstMacro( SpaceDimension ) >				Euler;
		// The following trick works though:
		template <unsigned int D>	
			class	EulerGroupWrap
		{
		public:
			typedef EulerGroup<D> Euler;
		};
		typedef EulerGroupWrap<Dimension> 							EulerGroupWrapInstance;
		typedef typename EulerGroupWrapInstance::Euler 	Euler;
		
		typedef typename Euler::Dummy< ScalarType >			EulerDummy;
		typedef typename EulerDummy::EulerTransform_tmp	EulerTransform_tmp;
		
	}; // end class EulerGroupTemplate
	
	/**
	 * ******************* class EulerTransform *********************
	 *
	 * This class
	 */
	
	template< class TScalarType, unsigned int Dimension >
		class EulerTransform:
	public EulerGroupTemplate<
		TScalarType, Dimension >::EulerTransform_tmp
	{
	public:

		/** Standard ITK-stuff.*/
		typedef EulerTransform											Self;
		typedef typename EulerGroupTemplate<
			TScalarType, Dimension >
			::EulerTransform_tmp											Superclass;
		typedef SmartPointer<Self>									Pointer;
		typedef SmartPointer<const Self>						ConstPointer;
		
		/** Method for creation through the object factory.*/
		itkNewMacro( Self );
		
		/** Run-time type information (and related methods).*/
		itkTypeMacro( EulerTransform, EulerGroupTemplate );
		
		/** Dimension of the domain space.*/
		itkStaticConstMacro( SpaceDimension, unsigned int, Dimension );
		
		/** Typedefs inherited from the superclass.*/

		/** These are both in Euler2D and Euler3D.*/
		typedef typename Superclass::ScalarType										ScalarType;
		typedef typename Superclass::ParametersType								ParametersType;
		typedef typename Superclass::JacobianType									JacobianType;
		typedef typename Superclass::OffsetType										OffsetType;
		typedef typename Superclass::InputPointType								InputPointType;
		typedef typename Superclass::OutputPointType							OutputPointType;
		typedef typename Superclass::InputVectorType							InputVectorType;
		typedef typename Superclass::OutputVectorType							OutputVectorType;
		typedef typename Superclass::InputCovariantVectorType			InputCovariantVectorType;
		typedef typename Superclass::OutputCovariantVectorType		OutputCovariantVectorType;
		typedef typename Superclass::InputVnlVectorType						InputVnlVectorType;
		typedef typename Superclass::OutputVnlVectorType					OutputVnlVectorType;
		
		/** NOTE: use these only in 3D (otherwise they are just int's).*/
		typedef typename EulerGroupTemplate<
			TScalarType, Dimension >::EulerDummy										EulerDummy;
		typedef typename EulerDummy::AngleType										AngleType;
		
	protected:

		EulerTransform(){};
		~EulerTransform(){};
		
	private:

		EulerTransform(const Self&); // purposely not implemented
		void operator=(const Self&); // purposely not implemented
		
	}; // end class EulerTransform
	
	
	
} // end namespace itk

#endif // end #ifndef __itkEulerTransform_H__

