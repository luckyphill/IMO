
#ifndef TumourCellBackgroundDeath_HPP_
#define TumourCellBackgroundDeath_HPP_

#include "AbstractCellKiller.hpp"
#include "RandomNumberGenerator.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>


/**
 * A cell killer that randomly kills cells based on the user set probability.
 *
 * The probability passed into the constructor will be the probability
 * of any cell dying whenever CheckAndLabelCellsForApoptosis() is called.
 *
 * Note this does take into account time steps - the input probability is the
 * probability that in an hour's worth of trying, the cell killer will have
 * successfully killed a given cell. In the method CheckAndLabelSingleCellForApoptosis()
 * this probability is used to calculate the probability that the cell is killed
 * at a given time step.
 *
 * We assume a constant time step and that there are an integer number (n = 1/dt)
 * of time steps per hour. We also assume that this method is called every time step
 * and that the probabilities of dying at different times are independent.
 */
template<unsigned DIM>
class TumourCellBackgroundDeath : public AbstractCellKiller<DIM>
{
private:

    /**
     * Probability that in an hour's worth of trying, the cell killer
     * will have successfully killed a given cell.
      */
     double mProbabilityOfDeathInAnHour;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the object.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellKiller<DIM> >(*this);

        // Make sure the random number generator is also archived
        SerializableSingleton<RandomNumberGenerator>* p_rng_wrapper = RandomNumberGenerator::Instance()->GetSerializationWrapper();
        archive & p_rng_wrapper;
    }

public:

    /**
     * Default constructor.
     *
     * @param pCellPopulation pointer to the cell population
     * @param probabilityOfDeathInAnHour probability that a cell is labelled for apoptosis in one hour's worth of trying
     */
    TumourCellBackgroundDeath(AbstractCellPopulation<DIM>* pCellPopulation, double probabilityOfDeathInAnHour);

    /**
     * @return mProbabilityOfDeathInAnHour.
     */
    double GetDeathProbabilityInAnHour() const;

    /**
     * Overridden method to test a given cell for apoptosis.
     *
     * @param pCell the cell to test for apoptosis
     */
    void CheckAndLabelSingleCellForApoptosis(CellPtr pCell);

    /**
     * Loop over cells and start apoptosis randomly, based on the user-set
     * probability.
     */
    void CheckAndLabelCellsForApoptosisOrDeath();

    /**
     * Overridden OutputCellKillerParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputCellKillerParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(TumourCellBackgroundDeath)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct a TumourCellBackgroundDeath.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const TumourCellBackgroundDeath<DIM> * t, const unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_cell_population = t->GetCellPopulation();
    ar << p_cell_population;
    double prob = t->GetDeathProbabilityInAnHour();
    ar << prob;
}

/**
 * De-serialize constructor parameters and initialise a TumourCellBackgroundDeath.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, TumourCellBackgroundDeath<DIM> * t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_cell_population;
    ar >> p_cell_population;
    double prob;
    ar >> prob;

    // Invoke inplace constructor to initialise instance
    ::new(t)TumourCellBackgroundDeath<DIM>(p_cell_population, prob);
}
}
} // namespace ...

#endif /*TumourCellBackgroundDeath_HPP_*/
