//
// Created by mikhail on 6/15/21.
//

#include <stdexcept>

#include "multiplicity.h"

int main( int n_args, char** args){
  if ( n_args < 3 ){
    throw std::runtime_error( "Error: minimum 2 arguments expected. Use ./convert input.list outfile.root" );
  }
  std::string input_list{ args[1] };
  std::string output_file{ args[2] };

  Multiplicity task;
  task.Init( input_list, output_file );
  task.Run();
  return 0;
}