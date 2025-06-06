#include "employeeRecord.h"

void exampleUsage() {
    // Employee with integer ID and double salary
    EmployeeRecord<int, double> emp1(101, "Alice Johnson", 65000.50);
    emp1.print();

    // Employee with string ID and float salary
    EmployeeRecord<QString, float> emp2("EMP-2023", "Bob Smith", 72000.0f);
    emp2.print();

    // Update salary example
    emp1.setSalary(70000.0);
    qDebug() << "Updated salary for" << emp1.getName() << ":" << emp1.getSalary();
}
