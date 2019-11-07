

#include "ChemoResistantCCM.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "RandomNumberGenerator.hpp"

ChemoResistantCCM::ChemoResistantCCM()
{
}

ChemoResistantCCM::~ChemoResistantCCM()
{
}

ChemoResistantCCM::ChemoResistantCCM(const ChemoResistantCCM& rModel)
    : AbstractPhaseBasedCellCycleModel(rModel),
    mBaseResistance(rModel.mBaseResistance),
    mDriftRate(rModel.mDriftRate)
{
    /*
     * The member variables mCurrentCellCyclePhase, mG1Duration,
     * mMinimumGapDuration, mStemCellG1Duration, mTransitCellG1Duration,
     * mSDuration, mG2Duration and mMDuration are initialized in the
     * AbstractPhaseBasedCellCycleModel constructor.
     *
     * The member variables mBirthTime, mReadyToDivide and mDimension
     * are initialized in the AbstractCellCycleModel constructor.
     *
     * Note that mG1Duration is (re)set as soon as InitialiseDaughterCell()
     * is called on the new cell-cycle model.
     */
}

void ChemoResistantCCM::Initialise()
{
    SetG1Duration();
    SetChemoResistance();
}

void ChemoResistantCCM::InitialiseDaughterCell()
{
    AbstractPhaseBasedCellCycleModel::InitialiseDaughterCell();
    SetG1Duration();
    SetChemoResistance();
}

void ChemoResistantCCM::SetG1Duration()
{
    assert(mpCell != nullptr);
    mG1Duration = RandomNumberGenerator::Instance()->NormalRandomDeviate(GetTransitCellG1Duration(), 2.0);

}


void ChemoResistantCCM::SetChemoResistance()
{
    assert(mDriftRate != DOUBLE_UNSET);

    if (mResistance == DOUBLE_UNSET)
    {
        mResistance = mBaseResistance;
    }
    else
    {
        double driftAmount = mDriftRate * RandomNumberGenerator::Instance()->StandardNormalRandomDeviate();
        mResistance += driftAmount;
    }

    if (mResistance < 0)
    {
        mResistance = 0;
    }

    if (mResistance > 1)
    {
        mResistance = 1;
    }
}

void ChemoResistantCCM::SetBaseChemoResistance(double baseResistance)
{
    
    mBaseResistance = baseResistance;
}

AbstractCellCycleModel* ChemoResistantCCM::CreateCellCycleModel()
{
    return new ChemoResistantCCM(*this);
}


    // A drift rate on the resistance
void ChemoResistantCCM::SetResistanceDrift(double driftRate)
{
    mDriftRate = driftRate;
}

void ChemoResistantCCM::ResetForDivision()
{
    AbstractPhaseBasedCellCycleModel::ResetForDivision();
    SetG1Duration();
}

double ChemoResistantCCM::GetResistanceLevel()
{
    return mResistance;
}

void ChemoResistantCCM::UpdateCellCyclePhase()
{
    double time_since_birth = GetAge();
    assert(time_since_birth >= 0);

    if (mpCell->GetCellProliferativeType()->IsType<DifferentiatedCellProliferativeType>())
    {
        mCurrentCellCyclePhase = G_ZERO_PHASE;
    }
    else if (time_since_birth < GetMDuration())
    {
        mCurrentCellCyclePhase = M_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration)
    {
        mCurrentCellCyclePhase = G_ONE_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration + GetSDuration())
    {
        mCurrentCellCyclePhase = S_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration + GetSDuration() + GetG2Duration())
    {
        mCurrentCellCyclePhase = G_TWO_PHASE;
    }
}

void ChemoResistantCCM::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
    // No new parameters to output, so just call method on direct parent class
    AbstractPhaseBasedCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}



