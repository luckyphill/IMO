

#ifndef ImmuneResponse_HPP_
#define ImmuneResponse_HPP_

#include "AbstractCellKiller.hpp"
#include "RandomNumberGenerator.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>


/**
 * A cell killer that randomly kills cells around the boundary of the tumour at a rate dependent on
 * either a user defined value, or a value dependent on the tumour volume
 */
template<unsigned DIM>
class ImmuneResponse : public AbstractCellKiller<DIM>
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

    bool mVolumeDeathRateOn;



public:

    /**
     * Default constructor.
     *
     * @param pCellPopulation pointer to the cell population
     * @param probabilityOfDeathInAnHour probability that a cell is labelled for apoptosis in one hour's worth of trying
     */
    ImmuneResponse(AbstractCellPopulation<DIM>* pCellPopulation, double probabilityOfDeathInAnHour);

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


    void SetBaseDeathRate(double baseDeathRate);

    std::vector<CellPtr> FindBoundaryCells();

    double CrossP(const CellPtr O, const CellPtr A, const CellPtr B);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ImmuneResponse)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct a ImmuneResponse.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const ImmuneResponse<DIM> * t, const unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_cell_population = t->GetCellPopulation();
    ar << p_cell_population;
    double prob = t->GetDeathProbabilityInAnHour();
    ar << prob;
}

/**
 * De-serialize constructor parameters and initialise a ImmuneResponse.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, ImmuneResponse<DIM> * t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_cell_population;
    ar >> p_cell_population;
    double prob;
    ar >> prob;

    // Invoke inplace constructor to initialise instance
    ::new(t)ImmuneResponse<DIM>(p_cell_population, prob);
}
}
} // namespace ...

#endif /*ImmuneResponse_HPP_*/
