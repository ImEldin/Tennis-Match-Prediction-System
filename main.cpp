#include <iostream>
#include <unordered_map>
#include <tuple>
#include <fstream>

using namespace std;

// hash funckija za tuple kod gema
struct gem_hash {
    template <typename T1, typename T2, typename T3>
    size_t operator()(const tuple<T1, T2, T3>& t) const {
        auto h1 = hash<T1>{}(get<0>(t));
        auto h2 = hash<T2>{}(get<1>(t));
        auto h3 = hash<T3>{}(get<2>(t));

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

//GLOBALNE VARIJABLE
typedef tuple<int, int, int> Stanje_gem;
unordered_map<Stanje_gem, double, gem_hash> sviGemovi;

// vjerovatnoca pobjede u gemu na osnovu trenuntog rezultata i vjerovatnoce osvajanja poena osobe koja servira
double dp_gem(int rez1, int rez2, double v_servis) {
    if(rez1 >= 4 && rez1-rez2 >= 2) return 1.0; // posto je gem gotov u korist igraca1 sanse da pobjedi su 1.0 ili 100%
    if(rez2 >= 4 && rez2-rez1 >= 2) return 0.0; // suprotno posto je gotov u korist igraca2 sanse su 0.0 ili 0%

    // za veoma velike rezultate u gemu kao u ovom slucaju 200:200 vjerovatnoca pobjede se aproksimira na 0.5 ili 50%
    if(rez1 >= 200 && rez2 >= 200) return 0.5;

    // trenutni rezultat u gemu
    Stanje_gem gem = make_tuple(rez1, rez2, (int)(v_servis * 10000));
    // ako vec imamo ovaj rezultat od ranije samo ga proslijedi
    if(sviGemovi.find(gem) != sviGemovi.end()) return sviGemovi[gem];

    // vjerovatnoca da igrac1 osvoji gem
    // citav gem se racuna po slijedecem principu: sta ako igrac1 osvoji poean; sta ako igrac2 osvoji poen
    // ovo se nastavlja sve do nekog od baznih slucajeva i tada se rekurzija vraca unazad i racuna vjerovatnoce za sve rezultate
    double vjerovatnoca = v_servis * dp_gem(rez1 + 1, rez2, v_servis) +
                          (1 - v_servis) * dp_gem(rez1, rez2 + 1, v_servis);
    return sviGemovi[gem] = vjerovatnoca;

}

// hash funckija za tuple kod tiebreakera
struct tiebreak_hash {
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    size_t operator()(const tuple<T1, T2, T3, T4, T5, T6>& t) const {
        auto h1 = hash<T1>{}(get<0>(t));
        auto h2 = hash<T2>{}(get<1>(t));
        auto h3 = hash<T3>{}(get<2>(t));
        auto h4 = hash<T4>{}(get<3>(t));
        auto h5 = hash<T5>{}(get<4>(t));
        auto h6 = hash<T6>{}(get<5>(t));

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5);
    }
};

//GLOBALNE VARIJABLE
typedef tuple<int, int, int, int, bool, int> Stanje_tieBreak;
unordered_map<Stanje_tieBreak, double, tiebreak_hash> sviTiebreak;

// posebna vrsta gema koji se zove tiebreak, logika je veoma slicna obicnom gemu samo se sada naizmjenicno mijenja ko servira
double dp_tiebreak(int rez1, int rez2, double v_servis_p1, double v_servis_p2, bool ko_servira, int zbir_poena) {
    if (rez1 >= 7 && rez1 - rez2 >= 2) return 1.0; // posto je gem gotov u korist igraca1 sanse da pobjedi su 1.0 ili 100%
    if (rez2 >= 7 && rez2 - rez1 >= 2) return 0.0; // suprotno posto je gotov u korist igraca2 sanse su 0.0 ili 0%

    // za veoma velike rezultate u gemu kao u ovom slucaju 200:200 vjerovatnoca pobjede se aproksimira na 0.5 ili 50%
    if (rez1 >= 200 && rez2 >= 200) return 0.5;

    // trenutni rezultat u gemu
    Stanje_tieBreak tie_break = make_tuple(rez1, rez2, (int)(v_servis_p1 * 10000), (int)(v_servis_p2 * 10000), ko_servira, zbir_poena);
    // ako vec imamo ovaj rezultat od ranije samo ga proslijedi
    if (sviTiebreak.find(tie_break) != sviTiebreak.end()) return sviTiebreak[tie_break];

    double vjerovatnoca = 0.0;

    // provjera izmjene servisa
    if (zbir_poena == 1) {
        ko_servira = !ko_servira; // nakon prvog servisa mijenja se ko servira
    }
    else if (zbir_poena % 2 != 0) {
        ko_servira = !ko_servira; // ako nije prvi servis svako dva servisa se mijenja ko servira
    }

    // rekurzivni pozivi na osnovu toga ko trenutno servira
    if (ko_servira) { // servira igrac1
        vjerovatnoca = v_servis_p1 * dp_tiebreak(rez1 + 1, rez2, v_servis_p1, v_servis_p2, ko_servira, zbir_poena + 1) +
                       (1 - v_servis_p1) * dp_tiebreak(rez1, rez2 + 1, v_servis_p1, v_servis_p2, ko_servira, zbir_poena + 1);
    }
    else { // servira igrac2
        vjerovatnoca = (1 - v_servis_p2) * dp_tiebreak(rez1 + 1, rez2, v_servis_p1, v_servis_p2, ko_servira, zbir_poena + 1) +
                       v_servis_p2 * dp_tiebreak(rez1, rez2 + 1, v_servis_p1, v_servis_p2, ko_servira, zbir_poena + 1);
    }

    return sviTiebreak[tie_break] = vjerovatnoca;
}

// hash funckija za tuple kod seta
struct set_hash {
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    size_t operator()(const tuple<T1, T2, T3, T4, T5, T6, T7>& t) const {
        auto h1 = hash<T1>{}(get<0>(t));
        auto h2 = hash<T2>{}(get<1>(t));
        auto h3 = hash<T3>{}(get<2>(t));
        auto h4 = hash<T4>{}(get<3>(t));
        auto h5 = hash<T5>{}(get<4>(t));
        auto h6 = hash<T6>{}(get<5>(t));
        auto h7 = hash<T7>{}(get<6>(t));

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6);
    }
};

//GLOBALNE VARIJABLE
typedef tuple<int, int, int, int, bool, int, int > Stanje_set;
unordered_map<Stanje_set, double, set_hash> sviSetovi;

double dp_set(int rez1, int rez2, double v_servis_p1, double v_servis_p2, bool ko_servira, int trenutni_gem_rez1 = 0, int trenutni_gem_rez2 = 0) {
    // Base cases: Check if the set is over
    if (rez1 >= 6 && rez1 - rez2 >= 2) return 1.0; // posto je set gotov u korist igraca1 sanse da pobjedi su 1.0 ili 100%
    if (rez2 >= 6 && rez2 - rez1 >= 2) return 0.0; // suprotno posto je gotov u korist igraca2 sanse su 0.0 ili 0%

    // ako se desi rezultat 6:6 poziva se funkcija za tiebreak
    if (rez1 == 6 && rez2 == 6) {
        return dp_tiebreak(trenutni_gem_rez1, trenutni_gem_rez2, v_servis_p1, v_servis_p2, ko_servira, trenutni_gem_rez1 + trenutni_gem_rez2);
    }

    // trenutni rezultat u setu
    Stanje_set set = make_tuple(rez1, rez2, (int)(v_servis_p1 * 10000), (int)(v_servis_p2 * 10000), ko_servira, trenutni_gem_rez1, trenutni_gem_rez2);
    // ako vec imamo ovaj rezultat od ranije samo ga proslijedi
    if (sviSetovi.find(set) != sviSetovi.end()) return sviSetovi[set];

    double vjerovatnoca = 0.0;

    // vjerovatnoca da igrac1 osvoji set
    // slicna logika kao u gemu samo ovdje gledamo vjerovatnocu da igrac osvoji gem kada servira kao i vjerovatnocu da izgubi gem kada servira
    // zatim vjerovatnocu da osvoji gem kada servira igrac2 kao i da izgubi gem kada servira igrac2
    if (ko_servira) { // true == servira igrac1
        // trenutni_gem_rez1 i trenutni_gem_rez2 su po defaulut 0 sto predstavlja pocetak novog gema
        // ali ako su postavljeni na neku vrijednost to predstavlja rezultat u gemu koji se trenutno igra
        vjerovatnoca = dp_gem(trenutni_gem_rez1, trenutni_gem_rez2, v_servis_p1) * dp_set(rez1 + 1, rez2, v_servis_p1, v_servis_p2, false) +
                       (1 - dp_gem(trenutni_gem_rez1, trenutni_gem_rez2, v_servis_p1)) * dp_set(rez1, rez2 + 1, v_servis_p1, v_servis_p2, false);
    }
    else { // false == servira igrac2
        vjerovatnoca = (1 - dp_gem(trenutni_gem_rez1, trenutni_gem_rez2, v_servis_p2)) * dp_set(rez1 + 1, rez2, v_servis_p1, v_servis_p2, true) +
                        dp_gem(trenutni_gem_rez1, trenutni_gem_rez2, v_servis_p2) * dp_set(rez1, rez2 + 1, v_servis_p1, v_servis_p2, true);
    }

    return sviSetovi[set] = vjerovatnoca;
}

// hash funckija za tuple kod matcha
struct match_hash {
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
    size_t operator()(const tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& t) const {
        auto h1 = hash<T1>{}(get<0>(t));
        auto h2 = hash<T2>{}(get<1>(t));
        auto h3 = hash<T3>{}(get<2>(t));
        auto h4 = hash<T4>{}(get<3>(t));
        auto h5 = hash<T5>{}(get<4>(t));
        auto h6 = hash<T6>{}(get<5>(t));
        auto h7 = hash<T7>{}(get<6>(t));
        auto h8 = hash<T8>{}(get<7>(t));
        auto h9 = hash<T9>{}(get<8>(t));
        auto h10 = hash<T10>{}(get<9>(t));

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7) ^ (h9 << 8) ^ (h10 << 9);
    }
};

//GLOBALNE VARIJABLE
typedef tuple<int, int, int, int, int, bool, int, int, int, int> Stanje_match;
unordered_map<Stanje_match, double, match_hash> sviMatches;

double dp_match(int rez1, int rez2, double v_servis_p1, double v_servis_p2, int broj_setova, bool ko_servira, int trenutni_set_rez1 = 0, int trenutni_set_rez2 = 0, int trenutni_gem_rez1 = 0, int trenutni_gem_rez2 = 0) {
    if(rez1 == (broj_setova / 2) + 1) return 1.0; // posto je match gotov u korist igraca1 sanse da pobjedi su 1.0 ili 100%
    if(rez2 == (broj_setova / 2) + 1) return 0.0; // suprotno posto je gotov u korist igraca2 sanse su 0.0 ili 0%

    // trenutni rezultat u setu
    Stanje_match match = make_tuple(rez1, rez2, (int)(v_servis_p1 * 10000), (int)(v_servis_p2 * 10000), broj_setova, ko_servira, trenutni_set_rez1, trenutni_set_rez2, trenutni_gem_rez1, trenutni_gem_rez2);
    // ako vec imamo ovaj rezultat od ranije samo ga proslijedi
    if (sviMatches.find(match) != sviMatches.end()) return sviMatches[match];

    double vjerovatnoca = 0.0;

    if (ko_servira) { // true == servira igrac1
        // trenutni_gem_rez1 i trenutni_gem_rez2 ovdje ne rade nist samo se proslijedjuju
        // trenutni_set_rez1 i trenutni_set_rez2 slicno kao u trenutni_gem_rez1 i trenutni_gem_rez2 predstavljaju rezultat u trenutnom setu koji se igra
        // na ovaj nacin zapravo mozemo racunati vjerovatnocu pobjede ako je mec u nekom odredjenom stanju a ne samo na pocetku
        vjerovatnoca = dp_set(trenutni_set_rez1, trenutni_set_rez2, v_servis_p1, v_servis_p2, true, trenutni_gem_rez1, trenutni_gem_rez2) * dp_match(rez1 + 1, rez2, v_servis_p1, v_servis_p2, broj_setova, false) +
                       (1 - dp_set(trenutni_set_rez1, trenutni_set_rez2, v_servis_p1, v_servis_p2, true, trenutni_gem_rez1, trenutni_gem_rez2)) * dp_match(rez1, rez2 + 1, v_servis_p1, v_servis_p2, broj_setova, false);
    }
    else { // false == servira igrac2
        vjerovatnoca = dp_set(trenutni_set_rez1, trenutni_set_rez2, v_servis_p1, v_servis_p2, false, trenutni_gem_rez2, trenutni_gem_rez1) * dp_match(rez1 + 1, rez2, v_servis_p1, v_servis_p2, broj_setova, true) +
                       (1 - dp_set(trenutni_set_rez1, trenutni_set_rez2, v_servis_p1, v_servis_p2, false, trenutni_gem_rez2, trenutni_gem_rez1)) * dp_match(rez1, rez2 + 1, v_servis_p1, v_servis_p2, broj_setova, true);
    }

    return sviMatches[match] = vjerovatnoca;
}

double Aproksimacija(int poeni, int servisi) {

    double alpha = 10.0;
    double prosjek = 0.6;

    double nazivnik = servisi + alpha;
    double brojnik = poeni + alpha * prosjek;
    return brojnik / nazivnik;
}

int main() {

    ifstream inFile("input.txt");
    if (!inFile.is_open()) {
        cerr << "Greska pri otvaranju fajla!" << endl;
        return 1;
    }

    cout << "PROGRAM ZA PREDIKCIJU TENISKOG MECA!" << endl;

    for (int scenario = 1; scenario <= 100; scenario++) {

        int broj_setova;
        int rez_match_1, rez_match_2;
        int rez_set_1,   rez_set_2;
        int rez_gem_1,   rez_gem_2;
        bool ko_servira;
        bool aproksimacija;


        inFile >> broj_setova;
        inFile >> rez_match_1 >> rez_match_2;
        inFile >> rez_set_1 >> rez_set_2;
        inFile >> rez_gem_1 >> rez_gem_2;
        inFile >> ko_servira;
        inFile >> aproksimacija;

        double v_servis_p1 = 0.0, v_servis_p2 = 0.0;
        if (aproksimacija) {
            inFile >> v_servis_p1;
            inFile >> v_servis_p2;
        }
        else {
            int poeni1, servisi1;
            int poeni2, servisi2;

            inFile >> poeni1 >> servisi1;
            inFile >> poeni2 >> servisi2;

            v_servis_p1 = Aproksimacija(poeni1, servisi1);
            v_servis_p2 = Aproksimacija(poeni2, servisi2);
        }

        cout << "----------------------------------------" << endl;
        cout << "SCENARIO #" << scenario << endl;

        cout << "Broj setova: " << broj_setova << endl;
        cout << "Trenutni rezultat u setovima: "
             << rez_match_1 << " " << rez_match_2 << endl;

        cout << "Rezultat u trenutnom setu: "
             << rez_set_1 << " " << rez_set_2 << endl;

        cout << "Rezultat u trenutnom gemu: "
             << rez_gem_1 << " " << rez_gem_2 << endl;

        cout << "Ko servira(1 za igraca 1 ili 0 za igraca 2): "
             << ko_servira << endl;

        cout << "Da li se vjeovatnoce aproksimiraju(0 za da ili 1 za ne): "
             << aproksimacija << endl;

        cout << "Vjerovatnoca da igrac1 uzme poen na svom servisu je: "
             << v_servis_p1 * 100 << "%" << endl;

        cout << "Vjerovatnoca da igrac2 uzme poen na svom servisu je: "
             << v_servis_p2 * 100 << "%" << endl;

        if (rez_gem_1 == 40) rez_gem_1 = 3;
        if (rez_gem_2 == 40) rez_gem_2 = 3;
        if (rez_gem_1 == 30) rez_gem_1 = 2;
        if (rez_gem_2 == 30) rez_gem_2 = 2;
        if (rez_gem_1 == 15) rez_gem_1 = 1;
        if (rez_gem_2 == 15) rez_gem_2 = 1;

        double vjerovatnoca = dp_match(
            rez_match_1, rez_match_2,
            v_servis_p1, v_servis_p2,
            broj_setova,
            ko_servira,
            rez_set_1, rez_set_2,
            rez_gem_1, rez_gem_2
        );

        cout << "Vjerovatnoca da igrac 1 pobjedi je: "
             << vjerovatnoca * 100 << "%" << endl << endl;
    }

    inFile.close();
    return 0;
}