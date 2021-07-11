#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$(ls $lists_dir | sed "${job_num}q;d")

cd $output_dir
mkdir -p $job_num
cd $job_num

while read line; do
    echo $line >> list.txt
done < $filelist
echo >> list.txt

echo "loading " $ownroot
source $ownroot

echo "executing $build_dir/convert $filelist output.root"

$build_dir/convert $filelist output.root

date $format
echo JOB FINISHED!