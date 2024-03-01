#pragma once
#include "GlobalFunctions.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>

//typedefs
typedef long long miliseconds;//stores miliseconds
typedef long long microseconds;//stores microseconds
typedef long double seconds;
typedef const miliseconds cmiliseconds;
typedef const microseconds cmicroseconds;
typedef const seconds cseconds;

//the time at which the application booted
extern std::chrono::time_point<std::chrono::steady_clock> boottime;

inline uint GetSecond()
{
	return (uint)time(NULL);
}

//DONT CONVERT TO INT!
//HUGE NUMBERS
inline microseconds getSeconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline microseconds getmicroseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline miliseconds getMiliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
inline seconds milisectosec(const miliseconds& ms)
{
	return (seconds)ms * 0.001;
}
inline seconds microsectosec(const microseconds& ms)
{
	return (seconds)ms * 0.000001;
}
inline miliseconds getMiliseconds(const seconds& s)
{
	return (miliseconds)(s * 1000);
}
inline miliseconds getMiliseconds(const microseconds& s)
{
	return (miliseconds)(s / 1000);
}

inline microseconds GetMicroSecondsSinceApplicationBoot()
{
	auto current_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(current_time - boottime).count();
}
inline miliseconds GetMiliSecondsSinceApplicationBoot()
{
	auto current_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(current_time - boottime).count();
}
inline seconds GetSecondsSinceApplicationBoot()
{
	return microsectosec(GetMicroSecondsSinceApplicationBoot());
}

// Converts UTC time string to a time_t value.

// Let's consider we are getting all the input in
// this format: '2014-07-25T20:17:22Z' (T denotes
// start of Time part, Z denotes UTC zone).
// A better approach would be to pass in the format as well.

inline std::time_t getEpochTime(const std::wstring& dateTime, const std::wstring dateTimeFormat = L"%Y-%m-%d %H:%M %p")
{

	// Create a stream which we will use to parse the string,
	// which we provide to constructor of stream to fill the buffer.
	std::wistringstream ss{ dateTime };

	// Create a tm object to store the parsed date and time.
	std::tm dt;

	// Now we read from buffer using get_time manipulator
	// and formatting the input appropriately.
	ss >> std::get_time(&dt, dateTimeFormat.c_str());

	// Convert the tm structure to time_t value and return.
	return std::mktime(&dt);
}