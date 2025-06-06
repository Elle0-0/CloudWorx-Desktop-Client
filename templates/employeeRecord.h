#ifndef EMPLOYEERECORD_H
#define EMPLOYEERECORD_H

#include <QString>
#include <QDebug>

// Template class for Employee Record
template<typename IDType, typename SalaryType>
class EmployeeRecord {
public:
    EmployeeRecord(IDType id, const QString& name, SalaryType salary)
        : m_id(id), m_name(name), m_salary(salary) {}

    void print() const {
        qDebug() << "Employee ID:" << m_id;
        qDebug() << "Employee Name:" << m_name;
        qDebug() << "Employee Salary:" << m_salary;
    }

    // Getters
    IDType getId() const { return m_id; }
    QString getName() const { return m_name; }
    SalaryType getSalary() const { return m_salary; }

    // Setters
    void setSalary(SalaryType salary) { m_salary = salary; }

private:
    IDType m_id;
    QString m_name;
    SalaryType m_salary;
};


#endif // EMPLOYEERECORD_H
