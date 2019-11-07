#!/bin/bash 
#SBATCH -p batch 
#SBATCH -N 1 
#SBATCH -n 1 
#SBATCH --time=01:00:00 
#SBATCH --mem=1GB 
#SBATCH --array=0-8500
# NOTIFICATIONS
#SBATCH --mail-type=ALL
#SBATCH --mail-user=phillip.j.brown@adelaide.edu.au

module load CMake
module load netCDF-Fortran
module unload OpenMPI
module unload HDF5

mkdir -p output


echo "array_job_index: $SLURM_ARRAY_TASK_ID" 
i=1 
found=0 
while IFS=, read bd id run
do 
    if [ $i = $SLURM_ARRAY_TASK_ID ]; then 
        echo "Running liver metastises, background death $bd, immune response $id"
        found=1 
        break 
    fi 
    i=$((i + 1)) 
done < death_test.txt 

if [ $found = 1 ]; then
	echo "/home/a1738927/fastdir/chaste_build/projects/LiverMetastic/test/TestLiverMets -sm 10 -run $run -id $id -bd $bd -t 1000 -bt 100"
    /home/a1738927/fastdir/chaste_build/projects/LiverMetastic/test/TestLiverMets -sm 10 -run $run -id $id -bd $bd -t 1000 -bt 100
else 
  echo "death_test.txt  does not have enough parameters for $SLURM_ARRAY_TASK_ID index" 
fi