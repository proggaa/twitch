#include <nlohmann/json.hpp>
#include <chrono>

#include <fstream>
#include <vector>

#include <iostream>
#include "reqs.h"
#include "base64.h"


using json = nlohmann::json;

std::string streamer = "twitch username";
std::string game = "Just Chatting";
std::vector<std::string> ips;
std::vector<std::string> ipsus;
int nexnAge = 0;
int nexnPrx = 0;
int lastProx = 0;
int lastdelProx = 0;
std::mutex __Threads;
std::mutex ofn_mutex;

void getWork(std::string prox, std::string uagent) {
    std::string streamer_id = "";

    std::string refz = "{\"operationName\":\"PlaybackAccessToken_Template\",\"query\":\"query PlaybackAccessToken_Template($login: String!, $isLive: Boolean!, $vodID: ID!, $isVod: Boolean!, $playerType: String!) {  streamPlaybackAccessToken(channelName: $login, params: {platform: \\\"web\\\", playerBackend: \\\"mediaplayer\\\", playerType: $playerType}) @include(if: $isLive) {    value    signature    __typename  }  videoPlaybackAccessToken(id: $vodID, params: {platform: \\\"web\\\", playerBackend: \\\"mediaplayer\\\", playerType: $playerType}) @include(if: $isVod) {    value    signature    __typename  }}\",\"variables\":{\"isLive\":true,\"login\":\"{streamer}\",\"isVod\":false,\"vodID\":\"\",\"playerType\":\"site\"}}";
    refz = std::regex_replace(refz, std::regex("\\{streamer\\}"), streamer);

    reqs we;
    we.proxy = prox;
    we.uagent = uagent;
    std::string re = we.requ("https://www.twitch.tv", "");
    if (we.device.size() < 5) {
        std::cout << prox+" error device \n";
        return;
    }
    if (we.session.size() < 5) {
        std::cout << prox+" error session \n";
        return;
    }
    re = we.requ("https://gql.twitch.tv/gql", refz);
    json j3;
    std::string value;
    std::string sign;
    try {
        j3 = json::parse(re);
        value = j3["data"]["streamPlaybackAccessToken"]["value"];
        sign = j3["data"]["streamPlaybackAccessToken"]["signature"];
    } catch(...) {
        std::cout << "error JSON \n";
        return;
    }


    {
        std::regex r("channel_id\\\\\":([0-9]+)");
        std::smatch m;
        std::regex_search(re, m, r);
        if (m.size() > 0) {
            streamer_id = m[1];
        }
    }

    std::cout << "{";
    std::cout << streamer_id;
    std::cout << "}\n";

    std::string newIp = std::to_string(macaron::Base64::randNum(1, 256));
    newIp += "." + std::to_string(macaron::Base64::randNum(1, 256));
    newIp += "." + std::to_string(macaron::Base64::randNum(1, 256));
    newIp += "." + std::to_string(macaron::Base64::randNum(1, 256));
    //value = std::regex_replace(value, std::regex("\"user_ip\":\"(\\S+)\","), "\"user_ip\":\""+ newIp +"\",");
    //std::cout << value;

    std::string hash1 = we.strrandom(32);
    std::string hash2 = we.strrandom(16);
    std::string hash3 = we.strrandom(16);
    std::string hash4 = we.strrandom(16);
    std::string hash5 = we.strrandom(16);
    std::string hash6 = we.strrandom(16);

    std::string newurl = "https://usher.ttvnw.net/api/channel/hls/{streamer}.m3u8?acmb=e30%3D&allow_source=true&fast_bread=true&p=360312&play_session_id=" + hash1;
    newurl += "&player_backend=mediaplayer&playlist_include_framerate=true&reassignments_supported=true&sig=";
    newurl += sign;
    newurl += "&&supported_codecs=avc1&token=";
    newurl += we.urlEncode(value);
    newurl += "&cdm=wv&player_version=1.15.0";
    newurl = std::regex_replace(newurl, std::regex("\\{streamer\\}"), streamer);
    re = we.requ(newurl, "");

    //we.proxy = "";

    std::string getUrl = "";
    {
        std::regex r("https://(\\S+)m3u8");
        std::smatch m;
        std::regex_search(re, m, r);
        if (m.size() > 1) {
            getUrl = m[0];
        }
    }

    std::stringstream f1(re.c_str());
    std::string s;
    std::string words1;
    while (getline(f1, s, '\n')) {
        if (s.find("#EXT-X-TWITCH-INFO") != std::string::npos) {
            words1 = s;
        }
    }

    std::vector<std::string> words2;
    std::stringstream f2(words1.c_str());
    std::string s2;
    while (getline(f2, s2, ',')) {
        s2 = std::regex_replace(s2, std::regex("\""), "");
        if (s2.find("=") != std::string::npos) {
            //std::cout << words[0];
            //std::cout << words[1];
            words2.push_back(s2);
        }
    }

    std::map<std::string, std::string> wordsAll;

    for (auto x : words2) {
        std::vector<std::string> words3;
        std::stringstream f33(x.c_str());
        std::string s33;
        while (getline(f33, s33, '=')) {
            
            words3.push_back(s33);
        }
        wordsAll[words3[0]] = words3[1];
    }

    std::string tsUrl = "";
    std::string guidid = macaron::Base64::uuid4();
    int minutes = 0;
    

    if (wordsAll.find("C") != wordsAll.end()) {
        tsUrl = wordsAll["C"];
        tsUrl = tsUrl + "";
        std::string B64out;
        int reee = macaron::Base64::Decode(tsUrl, B64out).length();
        if (reee) {
            std::cout << tsUrl;
            std::cout << "\n";
            std::cout << "error url base64";
            return;
        }
        tsUrl = B64out;
    }
    
    if (!streamer_id.size()) {
        std::cout << getUrl;
        std::cout << " streamer_id url\n";
        return;
    }
    if (!getUrl.size()) {
        std::cout << getUrl;
        std::cout << " get url\n";
        return;
    }
    if (!tsUrl.size()) {
        std::cout << "ts url\n";
        return;
    }

    std::string watched = "[{\"event\":\"minute-watched\",\"properties\":{\"app_session_id\":\"" + hash2 +
        "\",\"app_version\":\"" + guidid +
        "\",\"device_id\":\"" + we.device +
        "\",\"domain\":\"www.twitch.tv\",\"host\":\"www.twitch.tv\",\"platform\":\"web\",\"preferred_language\":\"ru-RU\",\"referrer_host\":\"\"," +
        "\"referrer_url\":\"\",\"received_language\":\"en\",\"tab_session_id\":\"" + hash3 +
        "\",\"batch_time\":{xtime},\"url\":\"https://www.twitch.tv/{streamer}\",\"client_time\":{xtime}.163,\"benchmark_server_id\":\"" + we.session +
        "\",\"bornuser\":true,\"browser\":\"5.0 (Windows)\",\"browser_family\":\"firefox\",\"browser_version\":\"107.0\",\"collapse_right\":true,\"collapse_left\":true,\"localstorage_device_id\":\"" + hash4 +
        "\",\"location\":\"channel\",\"page_session_id\":\"" + hash5 +
        "\",\"referrer\":\"\",\"referrer_domain\":\"\",\"session_device_id\":\"" + we.device +
        "\",\"theme\":\"light\",\"viewport_height\":947,\"viewport_width\":1134,\"channel\":\"{streamer}\",\"channel_id\":\"{streamer_id}\",\"game\":\"" + game +
        "\",\"hosted_game\":null,\"is_following\":false,\"is_live\":true,\"language\":\"ru\",\"average_bitrate\":5770785,\"backend\":\"mediaplayer\",\"broadcast_id\":\"" + wordsAll["BROADCAST-ID"] +
        "\",\"buffer_empty_count\":1,\"buffered_position\":721.978069,\"build_dist_id\":\"npm\",\"catch_up_mode\":\"speedup\",\"cluster\":\"" + wordsAll["CLUSTER"] +
        "\",\"core_version\":\"1.16.0-twitch.1-rc.4\",\"current_bitrate\":6781361,\"current_fps\":60,\"decoded_frames\":2684,\"dropped_frames\":0,\"estimated_bandwidth\":238259803,\"gap_skip_count\":0," +
        "\"gap_skip_duration\":0,\"hidden\":false,\"hls_latency_broadcaster\":4221,\"hls_latency_ingest\":4277," +
        "\"initial_buffer_duration\":1000,\"live\":true,\"low_latency\":true,\"manifest_cluster\":\"" + wordsAll["MANIFEST-CLUSTER"] +
        "\",\"manifest_node\":\"" + wordsAll["MANIFEST-NODE"] +
        "\",\"manifest_node_type\":\"weaver_cluster\",\"minutes_logged\":{minutes},\"muted\":true,\"node\":\"" + wordsAll["NODE"] +
        "\",\"origin_dc\":\"ams04\",\"os_name\":\"Windows\",\"os_version\":\"NT 10.0\",\"play_session_id\":\"" + hash1 +
        "\",\"playback_rate\":1,\"player\":\"site\",\"player_position\":718.017775,\"player_state\":\"Playing\",\"protocol\":\"HLS\",\"ptb\":0.0024679115316221354," +
        "\"quality\":\"auto\",\"rendered_frames\":2684,\"seconds_offset\":51.93742,\"serving_id\":\"" + wordsAll["SERVING-ID"] +
        "\",\"sink_buffer_size\":3.9834930896759033,\"sink_type\":\"mse\",\"stream_format\":\"chunked\",\"time\":{xtime}.147,\"transcoder_type\":\"transmux\",\"transport_download_bytes\":44477792,\"transport_download_duration\":38639,\"transport_first_byte_latency\":3281,\"transport_segment_duration\":61968,\"transport_segments\":31,\"user_agent\":\"" + we.uagent +
        "\",\"vid_display_height\":403,\"vid_display_width\":717,\"vid_height\":1080,\"vid_width\":1920,\"video_buffer_size\":3.960294,\"video_session_id\":\"" + wordsAll["VIDEO-SESSION-ID"] +
        "\",\"volume\":0.5,\"is_pbyp\":false,\"squad_stream_id\":null,\"squad_stream_session_id\":null,\"squad_stream_presentation_id\":null,\"is_mod\":false,\"time_spent_hidden\":487464,\"consent_comscore_ok\":true,\"app_fullscreen\":false," +
        "\"autoplayed\":true,\"backend_version\":\"1.16.0-twitch.1-rc.4\",\"broadcaster_software\":\"unknown_rtmp\",\"chat_visible\":false,\"content_mode\":\"live\",\"game_id\":\"509658\"," +
        "\"host_channel\":null,\"host_channel_id\":null,\"is_ad_playing\":false,\"logged_in\":true,\"login\":null,\"mse_support\":true," +
        "\"partner\":false,\"playback_gated\":false,\"player_size_mode\":\"default\",\"staff\":false,\"subscriber\":false,\"turbo\":false,\"user_id\":null," +
        "\"viewer_exemption_reason\":null,\"benchmark_session_id\":\"" + hash6 +
        "\",\"client_build_id\":\"" + guidid +
        "\",\"distinct_id\":\"" + we.device + "\",\"client_app\":\"twilight\"}}]";

    std::string post1 = "{\"event\":\"benchmark_template_loaded\",\"properties\":{\"app_version\":\"" + guidid +
        "\",\"benchmark_server_id\":\"" + we.session +
        "\",\"client_time\":{xtime}.956,\"device_id\":\"" + we.device + "\",\"duration\":257,\"url\":\"https://www.twitch.tv/{streamer}\"}}";
    post1 = std::regex_replace(post1, std::regex("\\{streamer\\}"), streamer);
    post1 = std::regex_replace(post1, std::regex("\\{xtime\\}"), std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()));

    we.requ(tsUrl, post1);

    for (int er = 0; er < 23; er++) {
        if (er >= 4) {
            return;
            minutes++;
            std::string xtime = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
            watched = std::regex_replace(watched, std::regex("\\{streamer\\}"), streamer);
            watched = std::regex_replace(watched, std::regex("\\{streamer_id\\}"), streamer_id);
            watched = std::regex_replace(watched, std::regex("\\{xtime\\}"), xtime);
            watched = std::regex_replace(watched, std::regex("\\{minutes\\}"), std::to_string(minutes));
            we.requ(tsUrl, "data="+macaron::Base64::Encode(watched));
            return;
            er = 0;
        }
        std::string geea = we.requ(getUrl, "");
        if (geea.find("OK") == std::string::npos) {
            std::cout << "error prox \n";
            return;
        }
        std::cout << er;
        std::cout << " req ";
        std::cout << prox;
        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    //

}

int randNum(int mmin, int mmax) {
    if (mmin >= mmax)
        return mmax;
    std::scoped_lock lock(ofn_mutex);
    std::random_device seeder;
    std::mt19937 rng(seeder());
    std::uniform_int_distribution<int> gen(mmin, mmax);
    return gen(rng);
}

std::string getPr(std::string url) {
    std::scoped_lock lock(__Threads);
    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - lastProx > 30) {
        lastProx = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        ips.clear();
        nexnPrx = 0;
    }
    if (ips.empty()) {
        nexnPrx = 0;
        reqs we;
        std::string raae = we.requ("https://listproxy.net/dfudfsiudfsiu.txt?type=1", "");
        std::stringstream fxa(raae);
        std::string s12;
        while (getline(fxa, s12, '\n')) {
            if (s12.size())
                ips.push_back(s12);
        }
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - lastdelProx > 300) {
        lastdelProx = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nexnPrx = 0;
        ipsus.clear();
    }
    
    while (true) {
        nexnPrx++;
        if (std::find(ipsus.begin(), ipsus.end(), ips[nexnPrx]) != ipsus.end()) {
            // v contains x 
        }
        else {
            ipsus.push_back(ips[nexnPrx]);
            return ips[nexnPrx];
        }
    }
    
    nexnPrx++;
    return ips[nexnPrx];
}

int main()
{

    std::ifstream t("D:/12.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::vector<std::string> agents;
    std::stringstream f1(buffer.str());
    std::string s;
    while (getline(f1, s, '\n')) {
        if (s.size() > 5)
            agents.push_back(s);
    }

    for (int i = 0; i < 180; i++) {
        //std::string proxy = ips[i];
        auto asyncDefault = std::thread([agents]() {
            while (true) {
                if (nexnAge >= agents.size()-1)
                    nexnAge = 0;
                getWork(getPr("https://listproxy.net/izdsicudcsiudsiuiu.txt?country=DE"), agents[nexnAge]);
                std::this_thread::sleep_for(std::chrono::milliseconds(177));
            }
        });
        asyncDefault.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(66));
    }
    std::this_thread::sleep_for(std::chrono::seconds(8600));
    
    return 0;
}





