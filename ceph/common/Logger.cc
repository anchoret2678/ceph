
#include "include/LogType.h"
#include "include/Logger.h"

#include <iostream>
#include "include/Clock.h"


Logger::Logger(string& fn, LogType *type)
{
  filename = "out/";
  filename += fn;
  interval = 1.0;
  start = last_logged = g_clock.gettime();  // time 0!
  wrote_header = false;
  open = false;
  this->type = type;
}

Logger::~Logger()
{
  flush(true);
  out.close();
}

long Logger::inc(char *s, long v)
{
  string key = s;
  return inc(key,v);
}
long Logger::inc(string& key, long v)
{
  if (!type->have_key(key)) 
	type->add_inc(key);
  flush();
  vals[key] += v;
  return vals[key];
}

long Logger::set(char *s, long v)
{
  string key = s;
  return set(key,v);
}
long Logger::set(string& key, long v)
{
  if (!type->have_key(key)) 
	type->add_set(key);

  flush();
  vals[key] = v;
  return vals[key];
}

long Logger::get(char *s)
{
  string key = s;
  return get(key);
}
long Logger::get(string& key)
{
  return vals[key];
}

void Logger::flush(bool force) 
{
  double now = g_clock.gettime();
  while (now >= last_logged + interval || force) {
	last_logged += interval;
	force = false;

	if (!open) {
	  out.open(filename.c_str(), ofstream::out);
	  open = true;
	  cout << "opening log file " << filename << endl;
	}

	// header?
	if (!wrote_header) {
	  out << "#";
	  for (vector<string>::iterator it = type->keys.begin(); it != type->keys.end(); it++) {
		out << "\t" << *it;
	  }
	  out << endl;
	  wrote_header = true;
	}

	// write line to log
	//out << (long)(last_logged - start);
	out << last_logged - start;
	for (vector<string>::iterator it = type->keys.begin(); it != type->keys.end(); it++) {
	  out << "\t" << get(*it);
	}
	out << endl;

	// reset the counters
	for (vector<string>::iterator it = type->inc_keys.begin(); it != type->inc_keys.end(); it++) 
	  set(*it, 0);
  }
}




