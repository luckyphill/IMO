
#ifndef ChemoResistantCCM_HPP_
#define ChemoResistantCCM_HPP_

#include "ChasteSerialization.hpp"
#include "ClassIsAbstract.hpp"
#include <boost/serialization/base_object.hpp>

#include "AbstractPhaseBasedCellCycleModel.hpp"

/**
// THis makes a phase based cell cycle model that can account for Chemo resistance

 */
class ChemoResistantCCM : public AbstractPhaseBasedCellCycleModel
{
private:

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the cell-cycle model.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractPhaseBasedCellCycleModel>(*this);
    }

    double mResistance = DOUBLE_UNSET;
    double mBaseResistance = DOUBLE_UNSET;
    double mDriftRate = DOUBLE_UNSET; // Will be an expectf

protected:

    /**
     * Subclasses can override this function if they wish, this just
     * allocates the default values for each of the different cell
     * types' G1 durations as defined in AbstractPhaseBasedCellCycleModel.
     */
    virtual void SetG1Duration();

    /**
     * Protected copy-constructor for use by CreateCellCycleModel.
     * The only way for external code to create a copy of a cell cycle model
     * is by calling that method, to ensure that a model of the correct subclass is created.
     * This copy-constructor helps subclasses to ensure that all member variables are correctly copied when this happens.
     *
     * This method is called by child classes to set member variables for a daughter cell upon cell division.
     * Note that the parent cell cycle model will have had ResetForDivision() called just before CreateCellCycleModel() is called,
     * so performing an exact copy of the parent is suitable behaviour. Any daughter-cell-specific initialisation
     * can be done in InitialiseDaughterCell().
     *
     * @param rModel the cell cycle model to copy.
     */
    ChemoResistantCCM(const ChemoResistantCCM& rModel);

public:

    /**
     * Default constructor - creates an ChemoResistantCCM.
     */
    ChemoResistantCCM();

    AbstractCellCycleModel* CreateCellCycleModel();

    /**
     * Destructor.
     */
    virtual ~ChemoResistantCCM();

    /** See AbstractPhaseBasedCellCycleModel::ResetForDivision() */
    virtual void ResetForDivision();

    /**
     * Default UpdateCellCyclePhase() method for a simple cell-cycle model.
     *
     * Can be overridden if they should do something more subtle.
     */
    virtual void UpdateCellCyclePhase();

    /**
     * Set the new cell's G1 duration once it has been created after division.
     * The duration will be based on cell type.
     */
    void InitialiseDaughterCell();

    /** See AbstractPhaseBasedCellCycleModel::Initialise() */
    virtual void Initialise();

    // This needs to be set in the same way G1 is set, to give a random drift to the resistance rate
    void SetChemoResistance();
    void SetBaseChemoResistance(double baseResistance);
    double GetResistanceLevel();

    // A drift rate on the resistance
    void SetResistanceDrift(double driftRate);

    /**
     * Overridden OutputCellCycleModelParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    virtual void OutputCellCycleModelParameters(out_stream& rParamsFile);
};

// Declare identifier for the serializer
#include "SerializationExportWrapper.hpp"
CHASTE_CLASS_EXPORT(ChemoResistantCCM)

#endif // ChemoResistantCCM_HPP_
