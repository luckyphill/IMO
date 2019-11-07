// Standard includes for tests
#include <cxxtest/TestSuite.h>
#include "AbstractCellBasedTestSuite.hpp"
#include "CheckpointArchiveTypes.hpp"
#include "SmartPointers.hpp"
#include "CommandLineArguments.hpp"

// Simulators
#include "OffLatticeSimulation.hpp"
#include "OffLatticeSimulationTooManyCells.hpp"

// Forces
// Fixed nonlinear force
#include "GeneralisedLinearSpringForce.hpp"
// #include "BasicNonLinearSpringForce.hpp"

// Mesh
#include "NodesOnlyMesh.hpp"

// Cell Population
#include "NodeBasedCellPopulation.hpp"

// Proliferative types
// Proliferative tumour celltype

//Cell cycle models
#include "NoCellCycleModel.hpp"
#include "UniformCellCycleModel.hpp"
#include "SimpleOxygenBasedCellCycleModel.hpp"
#include "SimpleOxygenBasedCellCycleModel.hpp"
// TumourDivision range

// Mutation State
#include "DefaultCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "WildTypeCellMutationState.hpp"


// Cell killers
#include "TumourCellBackgroundDeath.hpp"
#include "Chemotherapy.hpp"
#include "ImmuneResponse.hpp"


// Misc
#include "FakePetscSetup.hpp"
#include "Debug.hpp"

#include "CellCountWriter.hpp"

class TestLiverMetsOxygen : public AbstractCellBasedTestSuite
{
	
public:


	void TestGrowMetsOxygen() throw(Exception)
	{

		// ********************************************************************************************
		// Output control
		bool file_output = false;
		double sampling_multiple = 100000;
		if(CommandLineArguments::Instance()->OptionExists("-sm"))
		{   
			sampling_multiple = CommandLineArguments::Instance()->GetDoubleCorrespondingToOption("-sm");
			file_output = true;
			TRACE("File output occuring")

		}

		bool java_visualiser = false;
		if(CommandLineArguments::Instance()->OptionExists("-vis"))
		{   
			java_visualiser = true;
			TRACE("Java visulaiser ON")

		}

		double run_number = 1; // For the parameter sweep, must keep track of the run number for saving the output file
		if(CommandLineArguments::Instance()->OptionExists("-run"))
		{	
			run_number = CommandLineArguments::Instance()->GetDoubleCorrespondingToOption("-run");
			PRINT_VARIABLE(run_number)

		}

		double background = 0.005;
        if(CommandLineArguments::Instance()->OptionExists("-bd"))
        {
        	background = CommandLineArguments::Instance()->GetDoubleCorrespondingToOption("-bd");
        	PRINT_VARIABLE(background)
        }

        double immune = 0.05;
        if(CommandLineArguments::Instance()->OptionExists("-id"))
        {
        	immune = CommandLineArguments::Instance()->GetDoubleCorrespondingToOption("-id");
        	PRINT_VARIABLE(immune)
        }

        double burn_in_time = 100; // The time needed to clear the transient behaviour from the initial set up
        if(CommandLineArguments::Instance()->OptionExists("-bt"))
        {	
        	burn_in_time = CommandLineArguments::Instance()->GetUnsignedCorrespondingToOption("-bt");
        	PRINT_VARIABLE(burn_in_time)

        }

        double simulation_length = 100;
        if(CommandLineArguments::Instance()->OptionExists("-t"))
        {	
        	simulation_length = CommandLineArguments::Instance()->GetUnsignedCorrespondingToOption("-t");
        	PRINT_VARIABLE(simulation_length)

        }
		// ********************************************************************************************


		RandomNumberGenerator::Instance()->Reseed(run_number);
		std::vector<Node<2>*> nodes;
		std::vector<unsigned> location_indices;

		unsigned node_counter = 0;

		double dt = 0.01;

		double maxInteractionRadius = 1.5;

		Node<2>* first_node =  new Node<2>(node_counter,  false,  0, 0);
		nodes.push_back(first_node);
		location_indices.push_back(node_counter);
		node_counter++;
		

		NodesOnlyMesh<2> mesh;
		mesh.ConstructNodesWithoutMesh(nodes, maxInteractionRadius);

		std::vector<CellPtr> cells;

		MAKE_PTR(TransitCellProliferativeType, p_type);
		MAKE_PTR(WildTypeCellMutationState, p_state);

		{
			SimpleOxygenBasedCellCycleModel* p_cycle_model = new SimpleOxygenBasedCellCycleModel();
			// Just using the CI model because it has phase based stuff

			CellPtr p_cell(new Cell(p_state, p_cycle_model));
			p_cell->SetCellProliferativeType(p_type);
			
			// Need to get something out of this for the actual values
			p_cycle_model->SetMDuration(1);
			p_cycle_model->SetSDuration(1);
			p_cycle_model->SetTransitCellG1Duration(10);
			p_cycle_model->SetG2Duration(1);

			p_cycle_model->SetHypoxicConcentration(0.2);
			p_cycle_model->SetQuiescentConcentration(.6);
			p_cycle_model->SetCriticalHypoxicDuration(10000);

			p_cell->InitialiseCellCycleModel();

			cells.push_back(p_cell);
		}

		// ********************************************************************************************
		// Make the cell population
		NodeBasedCellPopulation<2> cell_population(mesh, cells, location_indices);
		// ********************************************************************************************

		cell_population.AddCellPopulationCountWriter<CellCountWriter>();

		// ********************************************************************************************
		// Make the simulation
		OffLatticeSimulationTooManyCells simulator(cell_population);
		simulator.SetDt(dt);
		simulator.SetSamplingTimestepMultiple(sampling_multiple);
		simulator.SetCellLimit(2000);


		std::stringstream simdir;
        simdir << "bd_" << background;
        simdir << "_id_" << immune;
        
        std::stringstream rundir;
        rundir << "run_" << run_number;

        std::stringstream timedir;
        timedir << "bt_" << burn_in_time;
        timedir << "_t_" << simulation_length;

        
        std::string output_directory = "TestLiverMets/" +  simdir.str() + "/"  + rundir.str() + "/" + timedir.str();

		simulator.SetOutputDirectory(output_directory);
		// ********************************************************************************************

		// ********************************************************************************************
		// File outputs
		// Files are only output if the command line argument -sm exists and a sampling multiple is set
		simulator.SetSamplingTimestepMultiple(sampling_multiple);
		// The java visuliser is set separately
		cell_population.SetOutputResultsForChasteVisualizer(java_visualiser);
		// ********************************************************************************************


		MAKE_PTR(GeneralisedLinearSpringForce<2>, p_force);

		simulator.AddForce(p_force);


		// ********************************************************************************************
		// Add in the cell killers
		MAKE_PTR_ARGS(TumourCellBackgroundDeath<2>, p_background_killer, (&cell_population, background));
		simulator.AddCellKiller(p_background_killer);
		// ********************************************************************************************

		// ********************************************************************************************
		// These two parameters are inately linked - the initial separation of the daughter nodes
		// and the initial resting spring length
		p_force->SetMeinekeDivisionRestingSpringLength( 1 );
		cell_population.SetMeinekeDivisionSeparation(0.05); // Set how far apart the cells will be upon division
		// ********************************************************************************************


		simulator.SetEndTime(burn_in_time);
		simulator.Solve();

		// MAKE_PTR_ARGS(Chemotherapy<2>, p_chemo, (&cell_population, 0.005));
		// simulator.AddCellKiller(p_chemo);

		TRACE("Adding Immune Response")
		MAKE_PTR_ARGS(ImmuneResponse<2>, p_immune, (&cell_population, immune));
		simulator.AddCellKiller(p_immune);

		simulator.SetEndTime(burn_in_time + simulation_length);
		simulator.Solve();

	};




}