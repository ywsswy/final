#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "YLog.h"
#include "curl/curl.h"

#pragma comment(lib,"libcurl.lib")  



static size_t DownloadCallback(char *buffer, size_t sz, size_t nmemb, void *writer){
	std::string* psResponse = (std::string*) writer;
	psResponse->append(buffer, sz * nmemb);
	return sz * nmemb;
}
CURLcode HTTPGet(const std::string &url, std::string &res){
	res = "";
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //œ‘ ΩœÍœ∏–≈œ¢
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback); 
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
	CURLcode res_code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res_code;
}
std::string UrlEncode(const std::vector<std::wstring> &vec_cs){
	std::wstring raw_cs = L"";
	for(std::vector<std::wstring>::const_iterator it = vec_cs.begin();it!=vec_cs.end();it++){
		raw_cs += *it;
		raw_cs += L"\r\n";
	}
	std::string result;
	for(unsigned int i = 0; i< static_cast<unsigned int>(raw_cs.length()); i++){
		unsigned char ch = unsigned char(raw_cs[i]);
		if(ch == ' '){
			result += '+';
		}else if(ch >= 'A' && ch <= 'Z'){
			result += ch;
		}else if(ch >= 'a' && ch <= 'z'){
			result += ch;
		}else if(ch >= '0' && ch <= '9'){
			result += ch;
		}else if(ch == '-' || ch == '-' || ch == '.' || ch == '!' || ch == '~' || ch == '*' || ch == '\'' || ch == '(' || ch == ')' ){
			result += ch;
		}else{
			result += "%";
			result += "0123456789abcdef"[unsigned int(ch) / 16];
			result += "0123456789abcdef"[unsigned int(ch) % 16];
		}
	}
	return result;
}

class YGlobal{
public:
	static std::vector<std::vector<int> > cs_yws_map;
	static int cs_yws_map_h;
	static int cs_yws_map_w;
	static int cs_yws_map_sr;//0s
	static int cs_yws_map_sc;
	static int cs_yws_map_nr;//0s
	static int cs_yws_map_nc;
};

std::vector<std::vector<int> > YGlobal::cs_yws_map;
int YGlobal::cs_yws_map_h;
int YGlobal::cs_yws_map_w;
int YGlobal::cs_yws_map_sr;//0s
int YGlobal::cs_yws_map_sc;
int YGlobal::cs_yws_map_nr;//0s
int YGlobal::cs_yws_map_nc;
void CsRawRead(const std::string &filename, std::vector<std::wstring> &vec_cs){
	std::wstring line = L"";
	std::wifstream if1(filename.c_str());
	while(getline(if1,line)){
		vec_cs.push_back(line);
	}
	return;
}
void CsInit(const int challenge, std::vector<std::wstring> &vec_cs){
	vec_cs.clear();
	std::string filename = "data/map/";
	std::stringstream filess;
	filess << filename << challenge;
	filess >> filename;
	std::ifstream if1(filename);
	if (!if1){
		assert(0 && "map_data_file load failed");
	}
	std::streambuf* oldbuf = std::cin.rdbuf(if1.rdbuf());
	std::wstring line = L"";
	std::wstringstream ss;
	std::cin >> YGlobal::cs_yws_map_h >> YGlobal::cs_yws_map_w;

	line = L"g_yws_map_r = ";
	ss.clear();
	ss << line << YGlobal::cs_yws_map_h;
	getline(ss,line);
	vec_cs.push_back(line);

	line = L"g_yws_map_c = ";
	ss.clear();
	ss << line << YGlobal::cs_yws_map_w;
	getline(ss,line);
	vec_cs.push_back(line);

	YGlobal::cs_yws_map.clear();
	for(int i = 0;i<YGlobal::cs_yws_map_h;i++){
		std::vector<int> bufrow;
		line = L"g_yws_map[";
		ss.clear();
		ss << line << i << "] = [";
		for(int j = 0;j<YGlobal::cs_yws_map_w;j++){
			int bufint = 0;
			std::cin >> bufint;
			bufrow.push_back(bufint);
			if(bufint == 10){
				YGlobal::cs_yws_map_sr = i;
				YGlobal::cs_yws_map_sc = j;
			}
			ss << bufint; 
			if(j != YGlobal::cs_yws_map_w-1){
				ss << ',';
			}else{
				ss << ']';
			}
		}
		YGlobal::cs_yws_map.push_back(bufrow);
		getline(ss,line);
		vec_cs.push_back(line);
	}
	line = L"g_yws_map_sr = ";
	ss.clear();
	ss << line << YGlobal::cs_yws_map_sr;
	getline(ss,line);
	vec_cs.push_back(line);
	line = L"g_yws_map_sc = ";
	ss.clear();
	ss << line << YGlobal::cs_yws_map_sc;
	getline(ss,line);
	vec_cs.push_back(line);
	std::cin.rdbuf(oldbuf);
	return;
}
YLog log1(YLog::INFO, "data/log1.txt",YLog::OVER);
YLog logjs(YLog::INFO, "data/logjs.txt",YLog::OVER);
int main(){
	std::vector<std::wstring> vec_cs_init;
	std::vector<std::wstring> vec_cs_user;
	std::vector<std::wstring> vec_cs_syst;
	CsRawRead("data/raw_cs_syst.txt",vec_cs_syst);
	CsInit(1,vec_cs_init);
	CsRawRead("data/raw_cs_user.txt",vec_cs_user);
	std::string encode_url = UrlEncode(vec_cs_syst)+UrlEncode(vec_cs_init)+UrlEncode(vec_cs_user);
	std::string strTmpStr;
	CURLcode res_code;
	std::string strUrl;
	
	strUrl = "https://111.230.151.212:85/?type=js1&query=" + encode_url;
	res_code = HTTPGet(strUrl,strTmpStr);
	if (res_code != CURLE_OK){
		log1.W("",0,YLog::INFO, "Error (CODE)", res_code);
	} else{
		log1.W("",0,YLog::INFO, "RETURN", strTmpStr);
	}
	strUrl = "https://111.230.151.212:85/?type=js&query=" + encode_url;
	res_code = HTTPGet(strUrl,strTmpStr);
	if (res_code != CURLE_OK){
		logjs.W("",0,YLog::INFO, "Error (CODE)", res_code);
	} else{
		logjs.W("",0,YLog::INFO, "RETURN", strTmpStr);
	}
	/*
	std::stringstream ss;
	ss << strTmpStr;
	std::string bufs;
	while(ss >> bufs){
		std::cout << bufs << '\n';
	}*/
	return 0;
}
