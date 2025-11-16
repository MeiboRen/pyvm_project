#include <iostream>
#include "vm/parser.hpp"
#include "vm/executor.hpp"

int main(int argc, char **argv) {
    if (argc > 1) {
        read_pyc stream(argv[1]);
        parser::initialize();
        parser parser(&stream);
        parser::main_code = parser.parse();
        executor *executor = executor::return_instance();
        executor->execute(parser::main_code);
        executor->end_execute();
    }
    else printf("usage: ./pyvm <pyc_file>\n");
    return 0;
}
