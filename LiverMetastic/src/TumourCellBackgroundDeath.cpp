

#include "TumourCellBackgroundDeath.hpp"
#include "Debug.hpp"

template<unsigned DIM>
TumourCellBackgroundDeath<DIM>::TumourCellBackgroundDeath(AbstractCellPopulation<DIM>* pCellPopulation, double probabilityOfDeathInAnHour)
        : AbstractCellKiller<DIM>(pCellPopulation),
          mProbabilityOfDeathInAnHour(probabilityOfDeathInAnHour)
{
    if ((mProbabilityOfDeathInAnHour<0) || (mProbabilityOfDeathInAnHour>1))
    {
        EXCEPTION("Probability of death must be between zero and one");
    }
}

template<unsigned DIM>
double TumourCellBackgroundDeath<DIM>::GetDeathProbabilityInAnHour() const
{
    return mProbabilityOfDeathInAnHour;
}

template<unsigned DIM>
void TumourCellBackgroundDeath<DIM>::CheckAndLabelSingleCellForApoptosis(CellPtr pCell)
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
void TumourCellBackgroundDeath<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin();
         cell_iter != this->mpCellPopulation->End();
         ++cell_iter)
    {
        CheckAndLabelSingleCellForApoptosis(*cell_iter);
    }
}

template<unsigned DIM>
void TumourCellBackgroundDeath<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<ProbabilityOfDeathInAnHour>" << mProbabilityOfDeathInAnHour << "</ProbabilityOfDeathInAnHour>\n";

    // Call method on direct parent class
    AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

// Explicit instantiation
template class TumourCellBackgroundDeath<1>;
template class TumourCellBackgroundDeath<2>;
template class TumourCellBackgroundDeath<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(TumourCellBackgroundDeath)
