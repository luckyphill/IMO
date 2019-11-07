
// A cell killer that kills the outer cells of a tumour representing the immune response

#include "Chemotherapy.hpp"
#include "ChemoResistantCCM.hpp"

template<unsigned DIM>
Chemotherapy<DIM>::Chemotherapy(AbstractCellPopulation<DIM>* pCellPopulation, double probabilityOfDeathInAnHour)
		: AbstractCellKiller<DIM>(pCellPopulation)
{
}



template<unsigned DIM>
void Chemotherapy<DIM>::SetBaseDeathRate(double baseDeathRate)
{
	mProbabilityOfDeathInAnHour = baseDeathRate;
}

template<unsigned DIM>
void Chemotherapy<DIM>::CheckAndLabelSingleCellForApoptosis(CellPtr pCell)
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

	ChemoResistantCCM* p_ccm = static_cast<ChemoResistantCCM*>(pCell->GetCellCycleModel());
	double resistanceLevel = p_ccm->GetResistanceLevel();
	double pDeath = mProbabilityOfDeathInAnHour * (tanh(2*(0.5- resistanceLevel)) + 1);
	double death_prob_this_timestep = 1.0 - pow((1.0 - pDeath), SimulationTime::Instance()->GetTimeStep());

	if (!pCell->HasApoptosisBegun() &&
		RandomNumberGenerator::Instance()->ranf() < death_prob_this_timestep)
	{
		pCell->StartApoptosis();
	}
}

template<unsigned DIM>
double Chemotherapy<DIM>::GetDeathProbabilityInAnHour() const
{
	return mProbabilityOfDeathInAnHour;
}

template<unsigned DIM>
void Chemotherapy<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
	
// Leave for now.
}

template<unsigned DIM>
void Chemotherapy<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
	*rParamsFile << "\t\t\t<ProbabilityOfDeathInAnHour>" << mProbabilityOfDeathInAnHour << "</ProbabilityOfDeathInAnHour>\n";

	// Call method on direct parent class
	AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

// Explicit instantiation
template class Chemotherapy<1>;
template class Chemotherapy<2>;
template class Chemotherapy<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(Chemotherapy)
