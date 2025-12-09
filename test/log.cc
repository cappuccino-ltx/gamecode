

#include "../comm/log.h"

int main() {

    infolog << "This is an info log.";
    debuglog << "This is a debug log.";
    warninglog << "This is a warning log.";
    errorlog << "This is an error log.";
    fatallog << "This is a fatal log.";


    infolog( "This is an info log.{} {} {}",1,"string",3.14);
    debuglog( "This is an debug log.{} {} {}",1,"string",3.14);
    warninglog( "This is an warning log.{} {} {}",1,"string",3.14);
    errorlog( "This is an error log.{} {} {}",1,"string",3.14);
    fatallog( "This is an fatal log.{} {} {}",1,"string",3.14);
}