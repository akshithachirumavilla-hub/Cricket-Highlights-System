#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

// ---------------- STRUCTS ----------------
struct Ball {
    string over;
    string event;
    int runs;
    string batsman;
};

struct Highlight {
    string over;
    string event;
    int score;
};

// ---------------- PARSE OVER ----------------
pair<int,int> parseOver(string s) {
    int over = 0, ball = 0, i = 0;

    while (s[i] != '.') {
        over = over * 10 + (s[i] - '0');
        i++;
    }
    i++;

    while (i < s.size()) {
        ball = ball * 10 + (s[i] - '0');
        i++;
    }

    return {over, ball};
}

// ---------------- ADVANCED RULE ENGINE ----------------
int getScore(string event, string over) {

    int base = 1;

    if (event == "WICKET") base = 10;
    else if (event == "CENTURY") base = 9;
    else if (event == "HALF_CENTURY") base = 8;
    else if (event == "SIX") base = 7;
    else if (event == "FOUR") base = 6;

    // Extract over number
    int overNum = stoi(over.substr(0, over.find('.')));

    // Pressure Boost
    if (overNum >= 16) base += 3;
    else if (overNum >= 10) base += 1;

    //  Clutch Moments
    if (event == "WICKET" && overNum >= 16) base += 2;
    if (event == "SIX" && overNum >= 18) base += 2;

    return base;
}

// ---------------- MAIN ----------------
int main() {

    cout << "====================================\n";
    cout << "IPL STYLE HIGHLIGHT GENERATOR\n";
    cout << "====================================\n";

    // -------- SAMPLE MATCH DATA --------
    vector<Ball> match = {
        {"1.1","1 RUN",1,"A"},
        {"1.2","FOUR",4,"A"},
        {"1.3","SIX",6,"A"},
        {"2.1","SIX",6,"B"},
        {"2.2","FOUR",4,"B"},
        {"3.1","WICKET",0,"C"},
        {"4.1","SIX",6,"A"},
        {"5.2","FOUR",4,"A"},
        {"6.3","SIX",6,"A"},
        {"7.1","WICKET",0,"D"},
        {"8.2","FOUR",4,"B"},
        {"9.1","SIX",6,"B"},
        {"10.1","1 RUN",1,"A"},
        {"11.1","2 RUNS",2,"A"},
        {"12.1","FOUR",4,"A"},
        {"13.1","SIX",6,"A"},
        {"14.1","1 RUN",1,"B"},
        {"15.1","WICKET",0,"E"},
        {"16.1","SIX",6,"B"},
        {"17.1","FOUR",4,"B"},
        {"18.1","SIX",6,"A"},
        {"19.1","FOUR",4,"A"},
        {"20.1","SIX",6,"B"}
    };

    vector<Highlight> all;

    map<string,int> batterRuns;
    map<string,bool> halfDone, centuryDone;

    // -------- GENERATE EVENTS --------
    for (auto b : match) {

        batterRuns[b.batsman] += b.runs;

        // CENTURY
        if (batterRuns[b.batsman] >= 100 && !centuryDone[b.batsman]) {
            all.push_back({b.over, b.batsman + "_CENTURY",
                           getScore("CENTURY", b.over)});
            centuryDone[b.batsman] = true;
        }
        // HALF CENTURY
        else if (batterRuns[b.batsman] >= 50 && !halfDone[b.batsman]) {
            all.push_back({b.over, b.batsman + "_HALF_CENTURY",
                           getScore("HALF_CENTURY", b.over)});
            halfDone[b.batsman] = true;
        }

        // MAIN EVENTS ONLY
        if (b.event == "SIX" || b.event == "FOUR" || b.event == "WICKET") {
            all.push_back({b.over, b.event,
                           getScore(b.event, b.over)});
        }
    }

    // -------- SORT BY SCORE --------
    sort(all.begin(), all.end(), [](Highlight a, Highlight b) {
        return a.score > b.score;
    });

    vector<Highlight> finalH;

    int maxK = 20;
    int sixCount = 0, fourCount = 0, wicketCount = 0;

    // -------- SMART FILTER --------
    for (auto h : all) {

        if (h.event == "SIX" && sixCount >= 6) continue;
        if (h.event == "FOUR" && fourCount >= 6) continue;
        if (h.event == "WICKET" && wicketCount >= 4) continue;

        finalH.push_back(h);

        if (h.event == "SIX") sixCount++;
        if (h.event == "FOUR") fourCount++;
        if (h.event == "WICKET") wicketCount++;

        if (finalH.size() == maxK) break;
    }

    // -------- FILL TO 20 --------
    for (auto h : all) {
        if (finalH.size() == maxK) break;

        bool exists = false;
        for (auto f : finalH) {
            if (f.over == h.over && f.event == h.event) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            finalH.push_back(h);
        }
    }

    // -------- SORT BY MATCH ORDER --------
    sort(finalH.begin(), finalH.end(), [](Highlight a, Highlight b) {
        auto p1 = parseOver(a.over);
        auto p2 = parseOver(b.over);

        if (p1.first == p2.first)
            return p1.second < p2.second;

        return p1.first < p2.first;
    });

    // -------- OUTPUT (IPL STYLE) --------
    cout << "\nMATCH HIGHLIGHTS\n\n";

    for (auto h : finalH) {

        int overNum = parseOver(h.over).first;

        // Phase labels
        if (h.over == "1.1") cout << "\n POWERPLAY (1-6 overs)\n";
        if (h.over == "7.1") cout << "\n MIDDLE OVERS (7-15)\n";
        if (h.over == "16.1") cout << "\n DEATH OVERS (16-20)\n";

        string text;

        if (h.event == "SIX")
            text = "SIX! Massive hit!";
        else if (h.event == "FOUR")
            text = " FOUR! Brilliant shot!";
        else if (h.event == "WICKET")
            text = " WICKET! Big breakthrough!";
        else if (h.event.find("HALF") != string::npos)
            text = " Half Century!";
        else if (h.event.find("CENTURY") != string::npos)
            text = " CENTURY! Outstanding!";
        else
            text = h.event;

        cout << "-> " << h.over << " | " << text << endl;
    }

    cout << "\n====================================\n";
    cout << " IPL STYLE HIGHLIGHTS READY!\n";
    cout << "====================================\n";

    return 0;
}