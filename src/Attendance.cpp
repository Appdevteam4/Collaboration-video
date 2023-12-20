#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

using namespace std;
using vec = vector<string>;
using matrix = vector<vec>;

//======================================================================

matrix readCSV(const string& filename)
{
    char separator = ',';
    matrix result;
    string row, item;

    ifstream in(filename);

    while (getline(in, row))
    {
        vec R;
        stringstream ss(row);
        while (getline(ss, item, separator))
            R.push_back(item);
        result.push_back(R);
    }

    in.close();
    return result;
}

//======================================================================

void printMatrix(const matrix& M)
{
    for (const vec& row : M)
    {
        for (const string& s : row)
            cout << setw(22) << left << s << " ";
        cout << '\n';
    }
}

//======================================================================

string calc_time(const string& time, int minutesToAdd)
{
    size_t colon_pos = time.find(':');
    if (colon_pos != string::npos)
    {
        int hours = stoi(time.substr(0, colon_pos));
        int minutes = stoi(time.substr(colon_pos + 1));
        minutes += minutesToAdd;

        if (minutes >= 60)
        {
            hours += 1;
            minutes -= 60;
        }

        stringstream updated_schedule;
        updated_schedule << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        return updated_schedule.str();
    }
    return ""; // Handle invalid time format
}

//======================================================================

string calc_time_5(const string& time)
{
    return calc_time(time, 5);
}

//======================================================================

string calc_time_15(const string& time)
{
    return calc_time(time, 15);
}

//======================================================================

string getCurrentDateTime()
{
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    struct tm* localTimeInfo = localtime(&currentTime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTimeInfo);
    return buffer;
}

//======================================================================

void add_date_to_output(matrix& M)
{
    string currentDate = "Attendance Checked: " + getCurrentDateTime();
    vec newRow = {currentDate};
    M.push_back(newRow);
}

//======================================================================

void check_attendance(matrix& M)
{
    string class_schedule = M[1][0];
    string teacherTimeIn;
    cout << "Class Schedule: " << class_schedule << endl;

    for (vec& row : M)
    {
        if (row.size() == 5 && row[2] == "Teacher")
        {
            teacherTimeIn = row[4];
            cout << "Updating attendance based on teacher's time in: " << teacherTimeIn << endl;

            if (teacherTimeIn.empty() || calc_time_15(class_schedule) < row[4])
            {
                if (row[4] != "Time")
                    row[4] = "Absent";
            }
            else if (calc_time_5(class_schedule) < row[4])
            {
                if (row[4] != "Time")
                    row[4] = "Late";
            }
            else
            {
                if (row[4] != "Time")
                    row[4] = "Present";
            }
        }
        else if (row.size() == 5)
        {
            string studentTimeIn = row[4];

            if (studentTimeIn.empty() || calc_time_15(teacherTimeIn) < studentTimeIn)
            {
                if (row[4] != "Time")
                    row[4] = "Absent";
            }
            else if (calc_time_5(teacherTimeIn) < studentTimeIn)
            {
                if (row[4] != "Time")
                    row[4] = "Late";
            }
            else
            {
                if (row[4] != "Time")
                    row[4] = "Present";
            }
        }
    }
    cout << "Attendance updated successfully!" << endl;
}

//======================================================================

void writeCSV(const matrix& M, const string& filename)
{
    ofstream out(filename, ios::app);

    if (!out.is_open())
    {
        cerr << "Error: Unable to create file: " << filename << endl;
        return;
    }

    for (const vec& row : M)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            out << row[i];
            if (i < row.size() - 1)
                out << ",";
        }
        out << '\n';
    }

    out.close();
}

//======================================================================

int main()
{
    matrix pets = readCSV("test.csv");
    printMatrix(pets);

    cout << "\n\n";

    check_attendance(pets);

    add_date_to_output(pets);

    printMatrix(pets);

    writeCSV(pets, "updated_attendance.csv");

    return 0;
}

