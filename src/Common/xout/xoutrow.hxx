#ifndef __xoutrow_hxx
#define __xoutrow_hxx

#include "xoutrow.h"


namespace xoutlibrary
{
	using namespace std;

	
	/**
	 * ********************* Constructor ****************************
	 */

	template< class charT, class traits >
		xoutrow<charT, traits>::xoutrow()
	{
		//nothing

	} // end Constructor


	/**
	 * ********************* Destructor *****************************
	 */

	template< class charT, class traits >
		xoutrow<charT, traits>::~xoutrow()
	{
		
		/** Call the function SetTargetCells, with as argument an empty
		 * XStreamMapType; In this way the memory that is managed by
		 * the cells in m_CellMap is properly deallocated */

		this->SetTargetCells( XStreamMapType() );

	} // end Destructor


	/**
	 * ******************** WriteBufferedData ***********************
	 * 
	 * This method can be overriden in inheriting classes. They 
	 * could for example define a specific order in which the
	 * cells are flushed.
	 */

	template< class charT, class traits >
		void xoutrow<charT, traits>::WriteBufferedData(void)
	{
		/** Write the cell-data to the outputs, separated by tabs */
		XStreamMapIteratorType xit;

		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			/** Write a tab to the cell */
			*(xit->second) <<	"\t" ;
			
			/** And send its contents to the outputs */
			xit->second->WriteBufferedData();
			
			/** the cell is empty now! */
			
		} // end for
				
		/** Go to the last cell and use it to send an enter to the outputs */
		--xit;

		*(xit->second) << "\n";

		xit->second->WriteBufferedData();
				
	} // end WriteBufferedData
	
	
	/**
	 * ******************** AddTargetCell ***************************
	 */

	template< class charT, class traits >
		int xoutrow<charT, traits>::
		AddTargetCell( const char * name )
	{		
		if ( m_CellMap.count( name ) == 0 )
		{		
			/** A new cell (type xoutcell) is created.*/
			XOutCellType * cell = new XOutCellType;

			/** Set the outputs equal to the outputs of this object */
			cell->SetOutputs( m_COutputs );
			cell->SetOutputs( m_XOutputs );

			/** Stored in a map, to make sure that later we can 
			 * delete all mnemory, assigned in this function. */
			m_CellMap.insert( XStreamMapEntryType( name, cell ) );
				
		}
		else 
		{
			return 1;
		}
	
		/** Add the pointer to the TargetCell-map. */
		return this->Superclass::AddTargetCell( name, m_CellMap[ name ] );

	} // end AddTargetCell


	/**
	 * ********************* RemoveTargetCell ***********************
	 */

	template< class charT, class traits >
		int xoutrow<charT, traits>::
		RemoveTargetCell( const char * name )
	{
		int returndummy = 1;

		if ( m_XTargetCells.count( name ) )
		{
			m_XTargetCells.erase( name );
			returndummy = 0;
		}
		
		if ( m_CellMap.count( name ) )
		{
			delete m_CellMap[ name ];
			m_CellMap.erase( name );
			returndummy = 0;
		}
		
		return returndummy;

	} // end RemoveTargetCell


	/**
	 * **************** SetTargetCells (xout objects) ***************
	 */

	template< class charT, class traits >
		void xoutrow<charT, traits>::
		SetTargetCells( const XStreamMapType & cellmap )
	{
		/** Clean the m_CellMap (cells that are created using the 
		 * AddTarget(const char *) method */

		XStreamMapIteratorType xit;

		for ( xit = m_CellMap.begin(); xit != m_CellMap.end(); ++xit )
		{
			delete xit->second;
		}
		
		m_CellMap.clear();

		/**	Replace the TargetCellMap with the input of this function.
		 * The outputs are not automatically set, because the user may
		 * want to keep the outputmap that was set in the cellmap */

		m_XTargetCells = cellmap;

	} // end SetTargetCells


	/**
	 * ****************** AddOutput (ostream_type) ******************
	 */

	template< class charT, class traits >
		int xoutrow<charT, traits>::
		AddOutput( const char * name, ostream_type * output )
	{
		int returndummy = 0;
		XStreamMapIteratorType xit;

		/** Set the output in all cells */
		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			returndummy |= xit->second->AddOutput( name, output );
		}

		/** Call the Superclass's implementation */
		returndummy |= this->Superclass::AddOutput( name, output );

		return returndummy;

	} // end AddOutput


	/**
	 * ********************** AddOutput (xoutbase) ******************
	 */

	template< class charT, class traits >
		int xoutrow<charT, traits>::
		AddOutput( const char * name, Superclass * output )
	{
		int returndummy = 0;
		XStreamMapIteratorType xit;

		/** Set the output in all cells */
		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			returndummy |= xit->second->AddOutput( name, output );
		}

		/** Call the Superclass's implementation */
		returndummy |= this->Superclass::AddOutput( name, output );

		return returndummy;

	} // end AddOutput


	/**
	 * ******************** RemoveOutput ****************************
	 */

	template< class charT, class traits >
		int xoutrow<charT, traits>::
		RemoveOutput( const char * name )
	{
		int returndummy = 0;
		XStreamMapIteratorType xit;

		/** Set the output in all cells */
		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			returndummy |= xit->second->RemoveOutput( name );
		}

		/** Call the Superclass's implementation */
		returndummy |= this->Superclass::RemoveOutput( name );

		return returndummy;

	} // end RemoveOutput


	/**
	 * ******************* SetOutputs (ostream_types) ***************
	 */

	template< class charT, class traits >
		void xoutrow<charT, traits>::
		SetOutputs( const CStreamMapType & outputmap )
	{		
		XStreamMapIteratorType xit;

		/** Set the output in all cells */
		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			xit->second->SetOutputs( outputmap );
		}

		/** Call the Superclass's implementation */
		this->Superclass::SetOutputs( outputmap );
		
	} // end SetOutputs

	
	/**
	 * ******************* SetOutputs (xoutobjects) *****************
	 */

	template< class charT, class traits >
		void xoutrow<charT, traits>::
		SetOutputs( const XStreamMapType & outputmap )
	{
		XStreamMapIteratorType xit;

		/** Set the output in all cells */
		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			xit->second->SetOutputs( outputmap );
		}

		/** Call the Superclass's implementation */
		this->Superclass::SetOutputs( outputmap );
	
	} // end SetOutputs


	/**
	 * ******************** WriteHeaders ****************************
	 */

	template< class charT, class traits >
		void xoutrow<charT, traits>::WriteHeaders(void)
	{
		/** Copy '*this' */
		Self headerwriter;
		headerwriter.SetTargetCells( m_XTargetCells ); //no CTargetCells, because they are not used in xoutrow!
		headerwriter.SetOutputs( m_COutputs );
		headerwriter.SetOutputs( m_XOutputs );

		/** Write the cell-names to the cells of the headerwriter.*/		
		XStreamMapIteratorType xit;

		for ( xit = m_XTargetCells.begin(); xit != m_XTargetCells.end(); ++xit )
		{
			/** Write the cell's name to each cell. */
			headerwriter[ xit->first.c_str() ] << xit->first;
						
		} // end for

		headerwriter.WriteBufferedData();
		
	} // end WriteHeaders
		

	/**
	 * ********************* SelectXCell ****************************
	 * 
	 * Returns a target cell.
	 */

	template< class charT, class traits >
		xoutbase<charT, traits> &
		xoutrow<charT, traits>::SelectXCell( const char * name )
	{		
		std::string cellname( name );

		/** Check if the name is "WriteHeaders". Then the method
		 * this->WriteHeaders() is invoked. */
		if ( cellname == "WriteHeaders" )
		{
			this->WriteHeaders();
			return *this;
		}
		else
		{
			/** Call the Superclass's implementation. */
			return this->Superclass::SelectXCell( name );
		}

	} // end SelectXCell


} // end namespace xoutlibrary


#endif // end #ifdef __xoutrow_hxx

