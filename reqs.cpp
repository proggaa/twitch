#include "reqs.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
{
	size_t newLength = size * nmemb;
	size_t oldLength = s->size();
	try
	{
		s->resize(oldLength + newLength);
	}
	catch (...)
	{
		return 0;
	}
	std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);
	return size * nmemb;
}

size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* stream = static_cast<FILE*>(userdata);
	if (!stream)
	{
		return 0;
	}
	size_t written = fwrite(static_cast<FILE*>(ptr), size, nmemb, stream);
	return written;
}

std::string reqs::requ(std::string url, std::string _data) {
	CURL* curl = curl_easy_init();
	CURLcode res;
	std::string result;
	std::string return_header;
	struct curl_slist* headers = nullptr;
	std::string uuaaa = "User-Agent: " + uagent;
	std::string headeDev = "X-Device-Id: " + device;


	headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
	headers = curl_slist_append(headers, "Accept-Language: en-US");
	headers = curl_slist_append(headers, "Connection: keep-alive");
	headers = curl_slist_append(headers, "Accept-Encoding: gzip,deflate");
	//headers = curl_slist_append(headers, "Host: gql.twitch.tv");
	//headers = curl_slist_append(headers, "Origin: https://www.twitch.tv");
	//headers = curl_slist_append(headers, "Referer: https://www.twitch.tv/");
	//headers = curl_slist_append(headers, "Host: www.twitch.tv");
	headers = curl_slist_append(headers, "sec-ch-ua-mobile: ?0");
	headers = curl_slist_append(headers, "sec-ch-ua-platform: \"Windows\"");
	headers = curl_slist_append(headers, "Sec-Fetch-Dest: empty");
	headers = curl_slist_append(headers, "Sec-Fetch-Mode: cors");
	headers = curl_slist_append(headers, "Sec-Fetch-Site: same-site");
	headers = curl_slist_append(headers, "_ResponseSource: Broswer");
	headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	headers = curl_slist_append(headers, "Client-Id: kimne78kx3ncx6brgo4mv6wki5h1ko");
	headers = curl_slist_append(headers, uuaaa.c_str());
	headers = curl_slist_append(headers, headeDev.c_str());

	std::string cooo = "Cookie: ";
	for (auto x : cookies) {
		cooo += x.first + "=";
		cooo += x.second + ";";
	}
	headers = curl_slist_append(headers, cooo.c_str());
	
	if (_data.size() > 1) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _data.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, _data.length());
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_HEADER, false);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 33L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 33L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &return_header);
	res = curl_easy_perform(curl);
	if (res == CURLE_OK) {
		//OutputDebugString("ok");
	}
	else {
		//OutputDebugString(curl_easy_strerror(res));
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	std::stringstream f1(return_header.c_str());
	std::string s;
	while (getline(f1, s, '\n')) {
		if (s.find("Set-Cookie: ") != std::string::npos) {
			s.erase(0, 12);
			std::stringstream f12(s);
			std::string s2;
			while (getline(f12, s2, ';')) {
				if (s2.find("=") == std::string::npos)
					continue;
				//std::cout << s2+"++++++++++++++\n";
				std::vector<std::string> params;
				std::stringstream f13(s2);
				std::string s3;
				while (getline(f13, s3, '=')) {
					params.push_back(s3);
				}
				if (params.size() == 2)
					cookies[params[0]] = params[1];
				break;
			}
		}
	}




	std::regex r("server_session_id=(\\S+);"); // entire match will be 2 numbers
	std::smatch m;
	std::regex_search(return_header, m, r);
	if (m.size() == 2) {
		session = m[1];
	}
	std::regex r2("unique_id=(\\S+);"); // entire match will be 2 numbers
	std::smatch m2;
	std::regex_search(return_header, m2, r2);
	if (m2.size() == 2) {
		device = m2[1];
	}

	std::string reez = result;
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);

	if (url.find("https://video-weaver") != std::string::npos) {
		if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK)
		{
			return "OK";
		}
	}

	return reez.c_str();
}

std::string reqs::urlEncode(std::string value) {
	CURL* curl = curl_easy_init();
	value = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.size()));
	curl_easy_cleanup(curl);
	return value;
}

std::string reqs::strrandom(int maxm) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(maxm);
	for (int i = 0; i < maxm; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return tmp_s;
}
