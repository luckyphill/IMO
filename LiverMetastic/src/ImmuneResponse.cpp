
// A cell killer that kills the outer cells of a tumour representing the immune response

#include "ImmuneResponse.hpp"
#include "AbstractCentreBasedCellPopulation.hpp"
#include "Debug.hpp"

template<unsigned DIM>
ImmuneResponse<DIM>::ImmuneResponse(AbstractCellPopulation<DIM>* pCellPopulation, double probabilityOfDeathInAnHour)
		: AbstractCellKiller<DIM>(pCellPopulation),
		mProbabilityOfDeathInAnHour(probabilityOfDeathInAnHour)
{
}

template<unsigned DIM>
double ImmuneResponse<DIM>::GetDeathProbabilityInAnHour() const
{
	double probability = 0;
	if (mVolumeDeathRateOn)
	{
		probability = mProbabilityOfDeathInAnHour;// some function of cell count * a rate
	}
	else
	{
		probability = mProbabilityOfDeathInAnHour;
	}
	return mProbabilityOfDeathInAnHour;
}


template<unsigned DIM>
void ImmuneResponse<DIM>::SetBaseDeathRate(double baseDeathRate)
{
	mProbabilityOfDeathInAnHour = baseDeathRate;
}

template<unsigned DIM>
void ImmuneResponse<DIM>::CheckAndLabelSingleCellForApoptosis(CellPtr pCell)
{
	/*
	 * We assume a constant time step and that there are an integer number (n = 1/dt)
	 * of time steps per hour. We also assume that this method is called every time step
	 * and that the probabilities of dying at different times are independent.
	 *
	 * Let q=mProbabilityOfDeathInAnHour and p="probability of death in a given time step".
	 *
	 * Probability of not dying in an hour:
	 * (1-q) = (1-p)^n = (1-p)^(1/dt).
	 *
	 * Rearranging for p:
	 * p = 1 - (1-q)^dt.
	 */
	double death_prob_this_timestep = 1.0 - pow((1.0 - mProbabilityOfDeathInAnHour), SimulationTime::Instance()->GetTimeStep());
	if (!pCell->HasApoptosisBegun() &&
		RandomNumberGenerator::Instance()->ranf() < death_prob_this_timestep)
	{
		pCell->StartApoptosis();
	}
}

template<unsigned DIM>
std::vector<CellPtr> ImmuneResponse<DIM>::FindBoundaryCells()
{
	AbstractCentreBasedCellPopulation<DIM,DIM>* p_tissue = static_cast<AbstractCentreBasedCellPopulation<DIM,DIM>*>(this->mpCellPopulation);
	// MeshBasedCellPopulation<DIM,DIM>* p_tissue = static_cast<MeshBasedCellPopulation<DIM,DIM>*>(this->mpCellPopulation);
	std::list<CellPtr> temp = p_tissue->rGetCells();

	std::vector<CellPtr> cells{ std::begin(temp), std::end(temp) };

	size_t n = cells.size(), k = 0;
	if (n <= 3) return cells;

	std::vector<CellPtr> H(2*n);

	// Sort points lexicographically by x value (then y if there are same x)
	sort(cells.begin(), cells.end(),
	[p_tissue](CellPtr A, CellPtr B)
	{
			double x_A = p_tissue->GetNodeCorrespondingToCell(A)->rGetLocation()[0];
			double y_A = p_tissue->GetNodeCorrespondingToCell(A)->rGetLocation()[1];

			double x_B = p_tissue->GetNodeCorrespondingToCell(B)->rGetLocation()[0];
			double y_B = p_tissue->GetNodeCorrespondingToCell(B)->rGetLocation()[1];
			// Node<2>* node_B = p_tissue->GetNodeCorrespondingToCell(B);
			return x_A < x_B || (x_A == x_B && y_A < y_B);
	});

	// Build lower hull
	for (size_t i = 0; i < n; ++i) {
		while (k >= 2 && CrossP(H[k-2], H[k-1], cells[i]) <= 0) k--;
		H[k++] = cells[i];
	}

	// Build upper hull
	for (size_t i = n-1, t = k+1; i > 0; --i) {
		while (k >= t && CrossP(H[k-2], H[k-1], cells[i-1]) <= 0) k--;
		H[k++] = cells[i-1];
	}

	H.resize(k-1);

	return H;

}

template<unsigned DIM>
double ImmuneResponse<DIM>::CrossP(const CellPtr O, const CellPtr A, const CellPtr B)
{
	AbstractCentreBasedCellPopulation<DIM,DIM>* p_tissue = static_cast<AbstractCentreBasedCellPopulation<DIM,DIM>*>(this->mpCellPopulation);
	double x_A = p_tissue->GetNodeCorrespondingToCell(A)->rGetLocation()[0];
	double y_A = p_tissue->GetNodeCorrespondingToCell(A)->rGetLocation()[1];

	double x_B = p_tissue->GetNodeCorrespondingToCell(B)->rGetLocation()[0];
	double y_B = p_tissue->GetNodeCorrespondingToCell(B)->rGetLocation()[1];

	double x_O = p_tissue->GetNodeCorrespondingToCell(O)->rGetLocation()[0];
	double y_O = p_tissue->GetNodeCorrespondingToCell(O)->rGetLocation()[1];


	return (x_A - x_O) * (y_B - y_O) - (y_A - y_O) * (x_B - x_O);
}

template<unsigned DIM>
void ImmuneResponse<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
	AbstractCentreBasedCellPopulation<DIM,DIM>* p_tissue = static_cast<AbstractCentreBasedCellPopulation<DIM,DIM>*>(this->mpCellPopulation);

	std::vector<CellPtr> boundaryCells = FindBoundaryCells();
	for (std::vector<CellPtr>::iterator it = boundaryCells.begin(); it != boundaryCells.end(); ++ it)
	{

		CheckAndLabelSingleCellForApoptosis(*it);
	}
}

template<unsigned DIM>
void ImmuneResponse<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
	*rParamsFile << "\t\t\t<ProbabilityOfDeathInAnHour>" << mProbabilityOfDeathInAnHour << "</ProbabilityOfDeathInAnHour>\n";

	// Call method on direct parent class
	AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

// Explicit instantiation
template class ImmuneResponse<1>;
template class ImmuneResponse<2>;
template class ImmuneResponse<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ImmuneResponse)
