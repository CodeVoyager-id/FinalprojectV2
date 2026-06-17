#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
 
using namespace std;

const int MAX_TUGAS = 100;
const int MAX_USERS = 10;

 
// Anggota 1
struct Tugas {
    int id;
    string namaTugas;
    string kategori;   
    int prioritas;     
    string deadline;   
    string status;     
    int estimasiWaktu;
    string pemilik;    
};
 
struct User {
    string username;
    string password;
};
 // Anggota 5 linked untuk Riwayat
struct NodeRiwayat{
string aktivitas;
NodeRiwayat* berukut;
};

NodeRiwayat* headRiwayat=NULL;
NodeRiwayat* tailRiwayat=NULL;

// Tambah riwayat ke akhir daftar
void tambahRiwayat(string aksi) {
    NodeRiwayat* nodeBaru = new NodeRiwayat;
    nodeBaru->aktivitas = "[" + currentUser + "] " + aksi;
    nodeBaru->berikut = NULL;

    if (kepalaRiwayat == NULL) {
        kepalaRiwayat = nodeBaru;
        ekorRiwayat = nodeBaru;
    } else {
        ekorRiwayat->berikut = nodeBaru;
        ekorRiwayat = nodeBaru;
    }
}
// Tampilkan riwayat milik pengguna aktif
void lihatRiwayatAksi() {
    if (kepalaRiwayat == NULL) {
        cout << "\n[Info] Belum ada riwayat aktivitas.\n";
        return;
    }
cout << "\n==================================\n";
    cout << "      RIWAYAT AKTIVITAS ANDA      \n";
    cout << "==================================\n";

    NodeRiwayat* bantu = kepalaRiwayat;
    int nomor = 0;
    bool ada = false;

    while (bantu != NULL) {
        if (bantu->aktivitas.find("[" + currentUser + "]") != string::npos) {
            cout << ++nomor << ". " << bantu->aktivitas << "\n";
            ada = true;
        }
        bantu = bantu->berikut;
    }

    if (!ada) {
        cout << "Tidak ada riwayat untuk akun ini.\n";
    }
    cout << "==================================\n";
}
// Bersihkan memori saat program selesai
void bersihkanMemoriRiwayat() {
    NodeRiwayat* bantu;
    while (kepalaRiwayat != NULL) {
        bantu = kepalaRiwayat;
        kepalaRiwayat = kepalaRiwayat->berikut;
        delete bantu;
    }
    ekorRiwayat = NULL;
}

// ==========================================
// PENYIMPANAN GLOBAL
// ==========================================
Tugas daftar[MAX_TUGAS];
User users[MAX_USERS];
int jumlahTugas = 0;
int jumlahUser = 0;
string currentUser = "";

 
struct LogRiwayat {
    string aktivitas;  // Catatan perubahan aksi user
};
 // FITUR UNDO MENGGUNAKAN STACK
//angota2
struct UndoData {
    Tugas data;
};

UndoData undoStack[MAX_TUGAS];
int topUndo = -1;

void pushUndo(Tugas t) {
    if (topUndo < MAX_TUGAS - 1) {
        undoStack[++topUndo].data = t;
    }
}

bool popUndo(Tugas &t) {
    if (topUndo == -1)
        return false;

    t = undoStack[topUndo--].data;
    return true;
}
void undoAksiTerakhir() {
    Tugas t;

    if (popUndo(t)) {
        daftar[jumlahTugas++] = t;

        cout << "\n[UNDO BERHASIL]\n";
        cout << "Tugas \"" << t.namaTugas
             << "\" berhasil dikembalikan.\n";
    } else {
        cout << "\nTidak ada data yang bisa di-Undo.\n";
    }
}
//angota2
// ================= QUEUE ANTREAN =================
struct QueueTugas {
    Tugas data;
};

QueueTugas antrean[MAX_TUGAS];
int frontQueue = 0;
int rearQueue = -1;

void enqueue(Tugas t) {
    if (rearQueue < MAX_TUGAS - 1) {
        antrean[++rearQueue].data = t;
    }
}
//  FITUR ANTREAN MENGGUNAKAN QUEUE
// anngota2

struct QueueTugas {
    Tugas data;
};

QueueTugas antrean[MAX_TUGAS];

int frontQueue = 0;
int rearQueue = -1;

void enqueue(Tugas t) {
    if (rearQueue < MAX_TUGAS - 1) {
        antrean[++rearQueue].data = t;
    }
}

bool dequeue(Tugas &t) {
    if (frontQueue > rearQueue)
        return false;

    t = antrean[frontQueue++].data;
    return true;
}

void buatAntreanPrioritas() {

    frontQueue = 0;
    rearQueue = -1;

    for (int p = 1; p <= 3; p++) {
        for (int i = 0; i < jumlahTugas; i++) {

            if (daftar[i].pemilik == currentUser &&
                daftar[i].prioritas == p &&
                daftar[i].status != "Selesai") {

                enqueue(daftar[i]);
            }
        }
    }

    cout << "\nAntrean tugas berhasil dibuat.\n";
}

void prosesAntrean() {

    Tugas t;

    if (dequeue(t)) {

        cout << "\nTugas Diproses:\n";
        cout << "ID        : " << t.id << endl;
        cout << "Nama Tugas: " << t.namaTugas << endl;

    } else {

        cout << "\nAntrean kosong.\n";
    }
}

bool dequeue(Tugas &t) {
    if (frontQueue > rearQueue)
        return false;

    t = antrean[frontQueue++].data;
    return true;
}
// Batasan Kapasitas Array Statis
const int MAX_TUGAS = 100;
const int MAX_USERS = 10;
const int MAX_RIWAYAT = 50;
 
// Penyimpanan Utama Global (Tugas Anggota 1)
Tugas daftar[MAX_TUGAS];
User users[MAX_USERS];
LogRiwayat riwayat[MAX_RIWAYAT];
 
int jumlahTugas = 0;
int jumlahUser = 0;
int jumlahRiwayat = 0;
string currentUser = ""; // Menyimpan user yang sedang login
 
 
int inputAngkaValid(const string& pesan, int min, int max) {
    int angka;
    while (true) {
        cout << pesan;
        cin >> angka;
        if (cin.fail() || angka < min || angka > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Input tidak valid! Masukkan angka antara " << min << " sampai " << max << ".\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return angka;
        }
    }
}
 
string inputTeksValid(const string& pesan) {
    string teks;
    while (true) {
        cout << pesan;
        getline(cin, teks);
        if (teks.empty()) {
            cout << "[Error] Input tidak boleh kosong!\n";
        } else {
            return teks;
        }
    }
}
 
void tambahRiwayat(string aksi) {
    if (jumlahRiwayat < MAX_RIWAYAT) {
        riwayat[jumlahRiwayat].aktivitas = "[" + currentUser + "] " + aksi;
        jumlahRiwayat++;
    } else {
        // Jika log riwayat penuh, geser lama ke kiri (Shifting)
        for (int i = 0; i < MAX_RIWAYAT - 1; i++) {
            riwayat[i] = riwayat[i + 1];
        }
        riwayat[MAX_RIWAYAT - 1].aktivitas = "[" + currentUser + "] " + aksi;
    }
}
 
int cariIndeksTugas(int id) {
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].id == id && daftar[i].pemilik == currentUser) {
            return i;
        }
    }
    return -1;
}
 
int cariIndeksUser(const string& username) {
    for (int i = 0; i < jumlahUser; i++) {
        if (users[i].username == username) {
            return i;
        }
    }
    return -1;
}
 
int dapatkanIdBaru() {
    int maxId = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].id > maxId) {
            maxId = daftar[i].id;
        }
    }
    return maxId + 1;
}
 
 

// FITUR FILE HANDLING (Save & Load)

 
//  SIMPAN DATA TUGAS ke tasks.txt 
void simpanTugas() {
    ofstream file("tasks.txt");
    if (!file) {
        cout << "[Error] Gagal menyimpan data tugas!\n";
        return;
    }
    for (int i = 0; i < jumlahTugas; i++) {
        file << daftar[i].id << "|"
             << daftar[i].namaTugas << "|"
             << daftar[i].kategori << "|"
             << daftar[i].prioritas << "|"
             << daftar[i].deadline << "|"
             << daftar[i].status << "|"
             << daftar[i].estimasiWaktu << "|"
             << daftar[i].pemilik << "\n";
    }
    file.close();
}
 
//  SIMPAN DATA USER ke users.txt 
void simpanUsers() {
    ofstream file("users.txt");
    if (!file) {
        cout << "[Error] Gagal menyimpan data user!\n";
        return;
    }
    for (int i = 0; i < jumlahUser; i++) {
        file << users[i].username << "|" << users[i].password << "\n";
    }
    file.close();
}
 
// MUAT DATA TUGAS dari tasks.txt 
void muatTugas() {
    ifstream file("tasks.txt");
    if (!file) return; // belum ada file -> mulai dari data kosong
 
    string baris;
    jumlahTugas = 0;
    while (getline(file, baris) && jumlahTugas < MAX_TUGAS) {
        if (baris.empty()) continue;
        stringstream ss(baris);
        string token;
        Tugas t;
 
        getline(ss, token, '|'); t.id = stoi(token);
        getline(ss, t.namaTugas, '|');
        getline(ss, t.kategori, '|');
        getline(ss, token, '|'); t.prioritas = stoi(token);
        getline(ss, t.deadline, '|');
        getline(ss, t.status, '|');
        getline(ss, token, '|'); t.estimasiWaktu = stoi(token);
        getline(ss, t.pemilik, '|');
 
        daftar[jumlahTugas++] = t;
    }
    file.close();
}
 
// MUAT DATA USER dari users.txt 
void muatUsers() {
    ifstream file("users.txt");
    if (!file) return; // belum ada file -> akun default akan dibuat
 
    string baris;
    jumlahUser = 0;
    while (getline(file, baris) && jumlahUser < MAX_USERS) {
        if (baris.empty()) continue;
        stringstream ss(baris);
        User u;
        getline(ss, u.username, '|');
        getline(ss, u.password, '|');
        users[jumlahUser++] = u;
    }
    file.close();
}
 
 
// CREATE
void tambahTugas() {
    if (jumlahTugas >= MAX_TUGAS) {
        cout << "\n[Gagal] Memori penuh! Tidak bisa menambahkan tugas lagi.\n";
        return;
    }
 
    Tugas tugasBaru;
    tugasBaru.id = dapatkanIdBaru(); // Auto-Increment ID
    tugasBaru.pemilik = currentUser;
 
    cout << "\n==================================\n";
    cout << "        TAMBAH TUGAS BARU         \n";
    cout << "==================================\n";
    cout << "ID Tugas Otomatis: " << tugasBaru.id << "\n";
    tugasBaru.namaTugas = inputTeksValid("Masukkan Nama Tugas/Aktivitas: ");
 
    int opsiKategori = inputAngkaValid("Pilih Kategori (1: Kuliah, 2: Pekerjaan, 3: Umum): ", 1, 3);
    if (opsiKategori == 1) tugasBaru.kategori = "Kuliah";
    else if (opsiKategori == 2) tugasBaru.kategori = "Pekerjaan";
    else tugasBaru.kategori = "Umum";
 
    tugasBaru.prioritas = inputAngkaValid("Masukkan Prioritas (1: Tinggi, 2: Sedang, 3: Rendah): ", 1, 3);
    tugasBaru.deadline = inputTeksValid("Masukkan Deadline (Format: YYYY-MM-DD / Hari Ini): ");
    tugasBaru.estimasiWaktu = inputAngkaValid("Estimasi Waktu Pengerjaan (dalam menit): ", 1, 1440);
    tugasBaru.status = "Belum Selesai";
 
    daftar[jumlahTugas++] = tugasBaru;
    simpanTugas(); //  simpan otomatis ke tasks.txt
 
    tambahRiwayat("Menambahkan tugas baru: " + tugasBaru.namaTugas);
    cout << "\n[Sukses] Tugas \"" << tugasBaru.namaTugas << "\" berhasil disimpan!\n";
}
 
// READ
void tampilkanTugas() {
    cout << "\n==================================\n";
    cout << "         TAMPILKAN TUGAS          \n";
    cout << "==================================\n";
 
    int hitungTugas = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik == currentUser) hitungTugas++;
    }
 
    if (hitungTugas == 0) {
        cout << "[Info] Belum ada tugas yang terdaftar untuk Anda.\n";
        return;
    }
 
    cout << "Pilih Mode Tampilan:\n";
    cout << "1. Tampilkan Semua Tugas\n";
    cout << "2. Filter Kategori: Kuliah\n";
    cout << "3. Filter Kategori: Pekerjaan\n";
    cout << "4. Filter Kategori: Umum\n";
    int pilihanFilter = inputAngkaValid("Pilih opsi (1-4): ", 1, 4);
 
    string filterKategori = "";
    if (pilihanFilter == 2) filterKategori = "Kuliah";
    else if (pilihanFilter == 3) filterKategori = "Pekerjaan";
    else if (pilihanFilter == 4) filterKategori = "Umum";
 
    int totalTugasTercetak = 0;
    cout << "\n--- DAFTAR TUGAS ANDA ---\n";
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik != currentUser) continue;
        if (pilihanFilter != 1 && daftar[i].kategori != filterKategori) continue;
 
        totalTugasTercetak++;
        cout << "ID Tugas   : " << daftar[i].id << "\n";
        cout << "Nama Tugas : " << daftar[i].namaTugas << "\n";
        cout << "Kategori   : " << daftar[i].kategori << "\n";
        cout << "Prioritas  : " << (daftar[i].prioritas == 1 ? "Tinggi" : (daftar[i].prioritas == 2 ? "Sedang" : "Rendah")) << "\n";
        cout << "Deadline   : " << daftar[i].deadline;
        if (daftar[i].deadline == "Hari Ini" || daftar[i].deadline == "hari ini") {
            cout << " [!!! MENDESAK !!!]";
        }
        cout << "\n";
        cout << "Estimasi   : " << daftar[i].estimasiWaktu << " Menit\n";
        cout << "Status     : [" << daftar[i].status << "]\n";
        cout << "-----------------------------------\n";
    }
 
    if (totalTugasTercetak == 0) {
        cout << "Tidak ada tugas dengan kategori \"" << filterKategori << "\".\n";
    }
}
 
// UPDATE
void updateTugas() {
    cout << "\n==================================\n";
    cout << "           UPDATE TUGAS           \n";
    cout << "==================================\n";
 
    int hitungTugas = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik == currentUser) hitungTugas++;
    }
    if (hitungTugas == 0) {
        cout << "[Info] Belum ada tugas yang bisa diperbarui.\n";
        return;
    }
 
    cout << "Pilih Mode Update:\n";
    cout << "1. Tandai Tugas Selesai (Berdasarkan ID)\n";
    cout << "2. Edit Detail Tugas Secara Total\n";
    cout << "3. Cari & Ganti Kata Kunci Nama Tugas (Search & Replace)\n";
    int pilihanUpdate = inputAngkaValid("Pilih opsi (1-3): ", 1, 3);
 
    if (pilihanUpdate == 1) {
        int cariID = inputAngkaValid("Masukkan ID tugas yang ingin ditandai selesai: ", 1, 100000);
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) {
            cout << "[Gagal] Tugas dengan ID " << cariID << " tidak ditemukan.\n";
            return;
        }
        if (daftar[indeks].status == "Selesai") {
            cout << "[Info] Tugas ini sudah berstatus 'Selesai'.\n";
        } else {
            daftar[indeks].status = "Selesai";
            simpanTugas(); // simpan otomatis ke tasks.txt
            tambahRiwayat("Menandai selesai tugas ID " + to_string(cariID));
            cout << "[Sukses] Tugas berhasil ditandai Selesai!\n";
        }
    } else if (pilihanUpdate == 2) {
        int cariID = inputAngkaValid("Masukkan ID tugas yang ingin diedit: ", 1, 100000);
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) {
            cout << "[Gagal] Tugas dengan ID " << cariID << " tidak ditemukan.\n";
            return;
        }
 
        daftar[indeks].namaTugas = inputTeksValid("Masukkan nama tugas baru: ");
        int opsiKategori = inputAngkaValid("Pilih kategori baru (1: Kuliah, 2: Pekerjaan, 3: Umum): ", 1, 3);
        if (opsiKategori == 1) daftar[indeks].kategori = "Kuliah";
        else if (opsiKategori == 2) daftar[indeks].kategori = "Pekerjaan";
        else daftar[indeks].kategori = "Umum";
        daftar[indeks].prioritas = inputAngkaValid("Masukkan prioritas baru (1: Tinggi, 2: Sedang, 3: Rendah): ", 1, 3);
        daftar[indeks].deadline = inputTeksValid("Masukkan deadline baru: ");
        daftar[indeks].estimasiWaktu = inputAngkaValid("Masukkan estimasi waktu baru (menit): ", 1, 1440);
        
        simpanTugas(); // simpan otomatis ke tasks.txt
        tambahRiwayat("Mengedit detail data pada tugas ID " + to_string(cariID));
        cout << "[Sukses] Data tugas berhasil diperbarui.\n";
    } else {
        string kataKunci = inputTeksValid("Masukkan kata kunci nama tugas yang ingin diganti: ");
        string teksBaru = inputTeksValid("Masukkan nama tugas baru sebagai penggantinya: ");
        int jumlahTerganti = 0;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik != currentUser) continue;
            if (daftar[i].namaTugas.find(kataKunci) != string::npos) {
                daftar[i].namaTugas = teksBaru;
                jumlahTerganti++;
            }
        }
        if (jumlahTerganti > 0) {
            simpanTugas(); // simpan otomatis ke tasks.txt
            tambahRiwayat("Melakukan Search & Replace nama kata kunci: " + kataKunci);
            cout << "[Sukses] Berhasil memperbarui " << jumlahTerganti << " tugas.\n";
        } else {
            cout << "[Info] Tidak ditemukan tugas dengan kata kunci tersebut.\n";
        }
    }
}
 
// DELETE
void hapusTugas() {
    cout << "\n==================================\n";
    cout << "           HAPUS TUGAS            \n";
    cout << "==================================\n";
 
    int hitungTugas = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik == currentUser) hitungTugas++;
    }
    if (hitungTugas == 0) {
        cout << "[Info] Belum ada tugas yang bisa dihapus.\n";
        return;
    }
 
    cout << "Pilih Mode Hapus:\n";
    cout << "1. Hapus Satu Tugas (Berdasarkan ID)\n";
    cout << "2. Bersihkan Semua Tugas Selesai (Massal)\n";
    int pilihanHapus = inputAngkaValid("Pilih opsi (1-2): ", 1, 2);
 
    if (pilihanHapus == 1) {
        int cariID = inputAngkaValid("Masukkan ID tugas yang ingin dihapus: ", 1, 100000);
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) {
            cout << "[Gagal] Tugas tidak ditemukan.\n";
            return;
        }
        // Menyimpan data ke Stack sebelum dihapus
        //angota2
        pushUndo(daftar[indeks]);
     
        // Shifting Array manual
        for (int i = indeks; i < jumlahTugas - 1; i++) {
            daftar[i] = daftar[i + 1];
        }
        jumlahTugas--;
        simpanTugas(); // simpan otomatis ke tasks.txt
        tambahRiwayat("Menghapus satu tugas ID " + to_string(cariID));
        cout << "[Sukses] Tugas berhasil dihapus.\n";
    } else {
        int jumlahTerhapus = 0;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik == currentUser && daftar[i].status == "Selesai") {
                for (int j = i; j < jumlahTugas - 1; j++) {
                    daftar[j] = daftar[j + 1];
                }
                jumlahTugas--;
                jumlahTerhapus++;
                i--;
            }
        }
        if (jumlahTerhapus > 0) {
            simpanTugas(); // [ANGGOTA 1] - simpan otomatis ke tasks.txt
            tambahRiwayat("Membersihkan massal seluruh tugas berstatus Selesai");
            cout << "[Sukses] " << jumlahTerhapus << " tugas selesai telah dibersihkan.\n";
        } else {
            cout << "[Info] Tidak ada tugas selesai untuk dihapus.\n";
        }
    }
}
 
// STATISTIK DATA
void tampilkanStatistik() {
    int total = 0, selesai = 0, belumSelesai = 0, totalEstimasi = 0;
    int kKuliah = 0, kPekerjaan = 0, kUmum = 0;
 
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik != currentUser) continue;
        total++;
        if (daftar[i].status == "Selesai") selesai++;
        else belumSelesai++;
        
        totalEstimasi += daftar[i].estimasiWaktu;
 
        if (daftar[i].kategori == "Kuliah") kKuliah++;
        else if (daftar[i].kategori == "Pekerjaan") kPekerjaan++;
        else if (daftar[i].kategori == "Umum") kUmum++;
    }
 
    cout << "\n==================================\n";
    cout << "      STATISTIK PRODUKTIVITAS      \n";
    cout << "==================================\n";
    cout << "Pengguna Aktif    : " << currentUser << "\n";
    cout << "Total Tugas Anda  : " << total << "\n";
    cout << "Tugas Selesai     : " << selesai << "\n";
    cout << "Tugas Belum       : " << belumSelesai << "\n";
    cout << "Total Sisa Waktu  : " << totalEstimasi << " Menit (~" << (totalEstimasi / 60.0) << " Jam)\n";
    cout << "----------------------------------\n";
    cout << "Sebaran Kategori Tugas:\n";
    cout << " - Kuliah    : " << kKuliah << "\n";
    cout << " - Pekerjaan : " << kPekerjaan << "\n";
    cout << " - Umum      : " << kUmum << "\n";
    cout << "==================================\n";
}
 
// RIWAYAT AKSI
void lihatRiwayatAksi() {
    cout << "\n==================================\n";
    cout << "      RIWAYAT AKTIVITAS ANDA      \n";
    cout << "==================================\n";
    
    int counter = 0;
    for (int i = 0; i < jumlahRiwayat; i++) {
        // Hanya menampilkan riwayat milik user aktif saat ini
        if (riwayat[i].aktivitas.find("[" + currentUser + "]") != string::npos) {
            cout << ++counter << ". " << riwayat[i].aktivitas << "\n";
        }
    }
    
    if (counter == 0) {
        cout << "[Info] Belum ada riwayat log aktivitas pada sesi ini.\n";
    }
    cout << "==================================\n";
}

// ================= ANGGOTA 4: FITUR SEARCHING & SORTING =================

// Fitur Cari (Searching) menggunakan Linear Search
void cariTugas() {
    cout << "\n==================================\n";
    cout << "           CARI TUGAS             \n";
    cout << "==================================\n";
    cout << "1. Cari Berdasarkan ID Tugas\n";
    cout << "2. Cari Berdasarkan Nama Tugas\n";
    int opsi = inputAngkaValid("Pilih Opsi Cari (1-2): ", 1, 2);

    bool ditemukan = false;

    if (opsi == 1) {
        int cariID = inputAngkaValid("Masukkan ID Tugas yang dicari: ", 1, 100000);
        cout << "\nHasil Pencarian:\n";
        for (int i = 0; i < jumlahTugas; i++) {
            // Memastikan ID cocok dan tugas tersebut milik user yang sedang login
            if (daftar[i].id == cariID && daftar[i].pemilik == currentUser) {
                // Panggil fungsi cetak (atau copas format tampilan Anggota 1 di sini)
                cout << "ID Tugas   : " << daftar[i].id << "\n";
                cout << "Nama Tugas : " << daftar[i].namaTugas << "\n";
                cout << "Kategori   : " << daftar[i].kategori << "\n";
                cout << "Prioritas  : " << (daftar[i].prioritas == 1 ? "Tinggi" : (daftar[i].prioritas == 2 ? "Sedang" : "Rendah")) << "\n";
                cout << "Deadline   : " << daftar[i].deadline << "\n";
                cout << "Estimasi   : " << daftar[i].estimasiWaktu << " Menit\n";
                cout << "Status     : [" << daftar[i].status << "]\n";
                cout << "-----------------------------------\n";
                ditemukan = true;
                break; // ID bersifat unik, jadi bisa langsung berhenti jika ketemu
            }
        }
    } else {
        string cariNama = inputTeksValid("Masukkan Kata Kunci Nama Tugas: ");
        cout << "\nHasil Pencarian:\n";
        for (int i = 0; i < jumlahTugas; i++) {
            // Mencari teks secara fleksibel menggunakan .find()
            if (daftar[i].pemilik == currentUser && daftar[i].namaTugas.find(cariNama) != string::npos) {
                cout << "ID Tugas   : " << daftar[i].id << "\n";
                cout << "Nama Tugas : " << daftar[i].namaTugas << "\n";
                cout << "Kategori   : " << daftar[i].kategori << "\n";
                cout << "Prioritas  : " << (daftar[i].prioritas == 1 ? "Tinggi" : (daftar[i].prioritas == 2 ? "Sedang" : "Rendah")) << "\n";
                cout << "Deadline   : " << daftar[i].deadline << "\n";
                cout << "Estimasi   : " << daftar[i].estimasiWaktu << " Menit\n";
                cout << "Status     : [" << daftar[i].status << "]\n";
                cout << "-----------------------------------\n";
                ditemukan = true;
            }
        }
    }

    if (!ditemukan) {
        cout << "[Info] Tugas tidak ditemukan atau bukan hak milik Anda.\n";
    }
}

// Fitur Urutkan (Sorting) menggunakan Bubble Sort
void urutkanTugas() {
    cout << "\n==================================\n";
    cout << "          URUTKAN TUGAS           \n";
    cout << "==================================\n";
    cout << "1. Urutkan berdasarkan Tingkat Prioritas (Tinggi -> Rendah)\n";
    cout << "2. Urutkan berdasarkan Estimasi Waktu (Cepat -> Lama)\n";
    int opsi = inputAngkaValid("Pilih Opsi Urutan (1-2): ", 1, 2);

    // Algoritma Bubble Sort untuk mengurutkan array statis
    for (int i = 0; i < jumlahTugas - 1; i++) {
        for (int j = 0; j < jumlahTugas - i - 1; j++) {
            bool harusTukar = false;

            if (opsi == 1) {
                // Prioritas 1 (Tinggi) harus berada di baris paling atas/awal array
                if (daftar[j].prioritas > daftar[j+1].prioritas) {
                    harusTukar = true;
                }
            } else {
                // Estimasi waktu paling kecil (cepat) ditaruh di awal array
                if (daftar[j].estimasiWaktu > daftar[j+1].estimasiWaktu) {
                    harusTukar = true;
                }
            }

            if (harusTukar) {
                Tugas temp = daftar[j];
                daftar[j] = daftar[j+1];
                daftar[j+1] = temp;
            }
        }
    }

    simpanTugas(); // Memanggil fungsi milik Anggota 1 untuk auto-save perubahan urutan
    tambahRiwayat("Mengurutkan daftar tugas melalui menu Sorting");
    cout << "\n[Sukses] Daftar tugas berhasil diurutkan secara permanen!\n";
    
    // Menampilkan hasil urutan terbaru ke layar
    cout << "\n--- DAFTAR TUGAS SETELAH DIURUTKAN ---\n";
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik == currentUser) {
            cout << "ID Tugas   : " << daftar[i].id << "\n";
            cout << "Nama Tugas : " << daftar[i].namaTugas << "\n";
            cout << "Prioritas  : " << (daftar[i].prioritas == 1 ? "Tinggi" : (daftar[i].prioritas == 2 ? "Sedang" : "Rendah")) << "\n";
            cout << "Estimasi   : " << daftar[i].estimasiWaktu << " Menit\n";
            cout << "Status     : [" << daftar[i].status << "]\n";
            cout << "-----------------------------------\n";
        }
    }
}
 
 
// USER SYSTEM LOGIN / REGISTER
bool loginMenu() {
    // Akun default awal 
    if (jumlahUser == 0) {
        users[jumlahUser++] = {"admin", "admin"};
        users[jumlahUser++] = {"user", "user"};
        simpanUsers(); // simpan akun default ke users.txt
    }
 
    while (true) {
        cout << "\n==================================\n";
        cout << "    SISTEM PRODUKTIVITAS LOGIN    \n";
        cout << "==================================\n";
        cout << "1. Masuk (Login Account)\n";
        cout << "2. Buat Akun Baru (Register)\n";
        cout << "3. Tutup Program\n";
        int pilihan = inputAngkaValid("Pilih Opsi (1-3): ", 1, 3);
 
        if (pilihan == 1) {
            string username = inputTeksValid("Username: ");
            string password = inputTeksValid("Password: ");
            int indeks = cariIndeksUser(username);
            
            if (indeks != -1 && users[indeks].password == password) {
                currentUser = username;
                cout << "\n[Sukses] Selamat datang kembali, " << currentUser << "!\n";
                return true;
            }
            cout << "[Error] Username atau sandi keliru.\n";
        } else if (pilihan == 2) {
            if (jumlahUser >= MAX_USERS) {
                cout << "[Error] Memori kuota user penuh.\n";
                continue;
            }
            string username = inputTeksValid("Daftar Username Baru: ");
            if (cariIndeksUser(username) != -1) {
                cout << "[Error] Nama user tersebut sudah terpakai.\n";
                continue;
            }
            string password = inputTeksValid("Daftar Password Baru: ");
            users[jumlahUser++] = {username, password};
            simpanUsers(); // simpan akun baru ke users.txt
            cout << "[Sukses] Akun \"" << username << "\" berhasil dibuat! Silakan gunakan menu Login.\n";
        } else {
            return false;
        }
    }
}
 
 

//  FUNGSI UTAMA (MAIN)

int main() {
    muatUsers(); // muat data user dari users.txt
    muatTugas();  // muat data tugas dari tasks.txt
 
    if (loginMenu()) {
        cout << "\nLogin berhasil. Lanjutkan ke menu utama aplikasi...\n";
    } else {
        cout << "\nProgram ditutup.\n";
    }
 
    return 0;
}
 
