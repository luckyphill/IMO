
#include "CellCountWriter.hpp"
#include "AbstractCellPopulation.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "CaBasedCellPopulation.hpp"
#include "NodeBasedCellPopulation.hpp"
#include "PottsBasedCellPopulation.hpp"
#include "VertexBasedCellPopulation.hpp"

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
CellCountWriter<ELEMENT_DIM, SPACE_DIM>::CellCountWriter()
    : AbstractCellPopulationCountWriter<ELEMENT_DIM, SPACE_DIM>("Nt.dat")
{
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::VisitAnyPopulation(AbstractCellPopulation<SPACE_DIM, SPACE_DIM>* pCellPopulation)
{
    std::list<CellPtr> cells = pCellPopulation->rGetCells();

    if (PetscTools::AmMaster())
    {
        *this->mpOutStream << cells.size();
    }
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::Visit(MeshBasedCellPopulation<ELEMENT_DIM, SPACE_DIM>* pCellPopulation)
{
    VisitAnyPopulation(pCellPopulation);
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::Visit(CaBasedCellPopulation<SPACE_DIM>* pCellPopulation)
{
    VisitAnyPopulation(pCellPopulation);
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::Visit(NodeBasedCellPopulation<SPACE_DIM>* pCellPopulation)
{
    VisitAnyPopulation(pCellPopulation);
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::Visit(PottsBasedCellPopulation<SPACE_DIM>* pCellPopulation)
{
    VisitAnyPopulation(pCellPopulation);
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
void CellCountWriter<ELEMENT_DIM, SPACE_DIM>::Visit(VertexBasedCellPopulation<SPACE_DIM>* pCellPopulation)
{
    VisitAnyPopulation(pCellPopulation);
}

// Explicit instantiation
template class CellCountWriter<1,1>;
template class CellCountWriter<1,2>;
template class CellCountWriter<2,2>;
template class CellCountWriter<1,3>;
template class CellCountWriter<2,3>;
template class CellCountWriter<3,3>;

#include "SerializationExportWrapperForCpp.hpp"
// Declare identifier for the serializer
EXPORT_TEMPLATE_CLASS_ALL_DIMS(CellCountWriter)
