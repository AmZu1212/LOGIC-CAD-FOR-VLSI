#include "hcmsigvec.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

// --------------------- static functions ---------------------
/** @fn static string busNodeName(string busName, int index)
 * @brief convert the signal name and index to the template - signal[index]
 * @param busName - the name of the signal
 * @param index - index of the signal
 * @return string represantion - signal[index]
 */
static string busNodeName(string busName, int index) {
  ostringstream result;
  result << busName << '[' << index << ']';
  return result.str();
}

/** @fn static string trim(const string& str, const string& whitespace = " \t\n")
 * @brief trim the given string from all white spaces (i.e trim.("\t\na[0:3]  \t") = a[0:3])
 * @param str - the string to trim
 * @param whitespace - the deliminters to trim from \astr\n optional parmeter, defult value = " \t\n"
 * @return a subset of the string without the whitespace in the begining and in the end
 */
static string trim(const string& str, const string& whitespace = " \n\r\t\f\v\0") {
  const size_t strBegin = str.find_first_not_of(whitespace);
  if (strBegin == string::npos)
    return ""; // no content
  
  const size_t strEnd = str.find_last_not_of(whitespace);
  const size_t strRange = strEnd - strBegin + 1;
  
  return str.substr(strBegin, strRange);
}
// --------------------- static functions ---------------------

hcmSigVec::hcmSigVec(string sigsFileName_, string vecsFileName_, bool verbose_)
  : verbose(verbose_ = false), isGood(true), vecLineNum(0), sigsFileName(sigsFileName_), vecsFileName(vecsFileName_) {
  
  sigs.open(sigsFileName.c_str());
  vecs.open(vecsFileName.c_str());
  
  if (!sigs.good()) {
    cerr << "-E- Failed opening Signal file: " << sigsFileName << endl;
	 isGood = false;
  }

  if (!vecs.good()) {
    cerr << "-E- Failed opening Vector file: " << vecsFileName << endl;
	 isGood = false;
  }

  if (isGood) {
	 if (parseSignalsFile()) {
		cerr << "-E- Failed to parse signals file: " << sigsFileName << endl;
		isGood = false;
	 }
  }
}

int hcmSigVec::getSigValue(string sigName, bool& val) {
  if (!isGood) {
	 cerr << "-E- getSigValue: But hcmSigVec object not initialized correctly." << endl;
	 return 1;
  }

  if (signalVecIdx.find(sigName) == signalVecIdx.end()) {
	 cerr << "-E- Could not find signal: " << sigName << endl;
	 return 1;
  }
  size_t idx = signalVecIdx[sigName];
  if (idx >= sigValsByIdx.size()) {
	 cerr << "-E- BUG signal: " << sigName << " idx: " << idx
			<< " >= sigValsByIdx.size() " << sigValsByIdx.size() << endl;
	 return 1;
  }
  val = sigValsByIdx[idx];
  return 0;
}

int hcmSigVec::getSignals(set<string>& signals) {
  for (map<string, int>::const_iterator it = signalVecIdx.begin(); it != signalVecIdx.end(); it++) {
	  signals.insert((*it).first);
  }
  return(signals.size());
}

int hcmSigVec::readVector() {
  if (vecs.eof()) {
    return(-1); 
  }

  string line;
  getline(vecs, line);
  string input_vector = trim(line);
  vecLineNum++;

  // the line is a long hexadecimal number as string.
  // we need to extract one bit at a time.

  // first lets check we have enough length.
  size_t strLen = input_vector.length();
  if (!strLen) {
    cerr << "-E- Empty line in vector files (line: " << vecLineNum << ")" << endl;
    return(1);
  }

  if (strLen < ((sigValsByIdx.size()+3)/4)) {
    cerr << "-E- Not enough hexadecimal digits (" << strLen
			<< " < " << ((sigValsByIdx.size()+3)/4) << ") in line:"
			<< vecLineNum << " = " << input_vector << endl;
    return(1);
  }

  // convert the line string to bits assigning them by index to each signal
  char c = input_vector[--strLen];
  unsigned digit = strtoul(&c, (char **) NULL, 16);
  for (unsigned int i = 0; i < sigValsByIdx.size(); i++) {
    sigValsByIdx[i] = digit & 1;
    if (verbose) {
      cout << "-D- Signal idx: " << i << " = " << sigValsByIdx[i] << " dig:" << digit << endl;
    }
    digit = digit >> 1;
    if ((i+1) % 4 == 0) {
      c = input_vector[--strLen];
      digit = strtoul(&c, (char **) NULL, 16);
      digit &= 0xff;
    }
  }
  return(0);
}

int hcmSigVec::parseSignalsFile() {
  string line;
  string prefix, fromStr, toStr;
  size_t lbrace, rbrace, colon;
  int idx = 0;
  while (sigs.good()) {
    getline(sigs, line);
    string name = trim(line);
    if (name.length() == 0) {
      continue;
    }
    lbrace = name.find("[");
    colon = name.find(":");
    rbrace = name.find("]");
    // the current signal is a bus (i.e a[0:3])
    if ((lbrace != string::npos) &&
		    (rbrace != string::npos) &&
		    (colon != string::npos)	) {
      // prefix = bus name (i.e a)
      prefix = name.substr(0, lbrace);
      // fromStr = left bound of bus (i.e 0)
      fromStr = name.substr(lbrace + 1, colon - lbrace - 1);
      // toStr = right bound of bus (i.e 3)
      toStr = name.substr(colon + 1, rbrace - colon - 1);
      int from = atoi(fromStr.c_str());
      int to = atoi(toStr.c_str());
      // check legal range of the bus
      if (from <= to) {
        // separate the bus range to single wire's  
        for (int i = to; i >= from; i--) { 
          string sig = busNodeName(prefix,i);
          if (verbose) { 
            cout << "-D- Signal: " << sig << " idx: " << idx << endl;
          }
          signalVecIdx[sig] = idx++;
        }
      } 
      else {
        for (int i = to; i <= from; i++) {
          string sig = busNodeName(prefix,i);
          if (verbose) {
            cout << "-D- Signal: " << sig << " idx: " << idx << endl;
          }
          signalVecIdx[sig] = idx++;
        }
      }
    } 
    // the current signal is a wire (i.e clk)
    else {
      if (verbose) {
		    cout << "-D- Signal: " << name << " idx: " << idx << endl;
      }
      signalVecIdx[name] = idx++;
    }
  }

  // initialize false value for all signals
  sigValsByIdx.resize(signalVecIdx.size(), false);
  
  return(0);
}
