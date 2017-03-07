#include <string>
#include <vector>
#include <iostream>
#include "curl.h"
#include "curlbuild.h"
#include "curlrules.h"
#include "curlver.h"
#include "easy.h"
#include "mprintf.h"
#include "multi.h"
#include "stdcheaders.h"
#include "typecheck-gcc.h"
#include <fstream>
#include <sstream>

struct Value{
  virtual ~Value() = default;
  virtual int weight() const{
    return 1;
  }
};

struct Number:Value{
  float num;
  int weight() const{
    return 1;
  }
};

struct Null:Value{
};

struct Bool:Value{
  bool val;
  int weight() const{
    return 1;
  }
};

struct String:Value, std::string{
  using std::string::string;
};

struct Array:Value, std::vector<Value*>{
  using std::vector<Value*>::vector;
  ~Array(){
    for(Value* v : *this)
      delete v;
  }
  int weight() const{
    int n = 0;
    for(Value* v : *this)
      n += v -> weight();
    return n;
  }
};

struct Object:Value, std::unordered_map<std::string, Value*>{
  using unordered_map<std::srting, Value*>::unordered_map;
};

Value* parse(char* f, char* l){
  if(f == "{")
    parse_object();
  if(f == "[")
    parse_array();
  if(f == "\"")
    parse_string();
  if(f == "T")
    parse_true();
  if(f == "F")
    parse_false();
  if(f == "N")
    parse_null();
  if(f == isdigit(f.c_str()))
    parse_number();
  if(f == " ")
    //skip/ignore space?
}

Value* parse_array(char* f, char* l){
  ++f; //first char is  a '[' so skip this
  Array* A = new Array;
  while(true)
    Value* v = parse(f, l);

}

//taken from http://www.cplusplus.com/forum/unices/45878/
// callback function writes data to a std::ostream
static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp){
	if(userp){
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if(os.write(static_cast<char*>(buf), len))
			return len;
	}
	return 0;
}

//taken from http://www.cplusplus.com/forum/unices/45878/
CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30){
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if(curl){
		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))){
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}

int main(){

  //taken from http://www.cplusplus.com/forum/unices/45878/
  curl_global_init(CURL_GLOBAL_ALL);
	std::ofstream ofs("json.txt");
  std::string jsonInput;
	if(CURLE_OK == curl_read("http://reddit.com/r/front.json", ofs)){
		// Web page successfully written to file
	}
	std::ostringstream oss;
	if(CURLE_OK == curl_read("http://reddit.com/r/front.json", oss)){
		// Web page successfully written to string
		std::string jsonStr = oss.str();
	}
	if(CURLE_OK == curl_read("http://reddit.com/r/front.json", std::cout)){
		// Web page successfully written to standard output (console?)
	}
	curl_global_cleanup();

  return 0;
}
