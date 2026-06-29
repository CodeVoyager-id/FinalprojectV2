#include <iostream>
#include <limits>
#include <fstream>
#include <string> 

// --- DETEKSI OTOMATIS SISTEM OPERASI (CROSS-PLATFORM) ---
#if defined(_WIN32) || defined(_WIN64)
    #include <conio.h>     
    #define IS_WINDOWS 1
#else
    #include <termios.h>   
    #include <unistd.h>
    #define IS_WINDOWS 0
#endif

using namespace std;

// ============================================================================
// PART 1: DEKLARASI DATA, STRUCT & POINTER UTAMA
// ============================================================================

// [ANGGOTA 1] - Representasi Entitas Data Utama & Batasan Array
const int MAX_TUGAS = 100;
const int MAX_USERS = 10;
string currentUser = ""; 

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

// [ANGGOTA 1] - Pointer untuk Array Dinamis Penyimpanan Utama
Tugas* daftar = new Tugas[MAX_TUGAS]; 
int jumlahTugas = 0;

// [ANGGOTA 2] - Struct Pengguna untuk Autentikasi File
struct User {
    string username;
    string password;
};
User users[MAX_USERS];
int jumlahUser = 0;

// [ANGGOTA 5] - Struct Riwayat Aktivitas Dinamis
struct NodeLog {
    string aktivitas;
    NodeLog* next;
};
NodeLog* headLog = nullptr; // Head Linked List


// ============================================================================
// PART 2: UTILITAS UTAMA, INPUT VALIDASI, & LOG LINKED LIST (ANGGOTA 5)
// ============================================================================

// [ANGGOTA 5] - Pembersihan Layar & Tampilan Header
void tampilkanHeaderKotak() {
    #if IS_WINDOWS
        system("cls");
    #else
        system("clear");
    #endif
    cout << "+=======================================================+\n";
    cout << "|                       APEX TASK                       |\n";
    cout << "|         SISTEM MANAJEMEN TUGAS & PRODUCTIVITY         |\n";
    cout << "+=======================================================+\n";
}

// [ANGGOTA 5] - Tampilan Indikator Sesi User
void tampilkanUserActive(const string& username) {
    cout << " +-----------------------------------------+\n";
    cout << " |  USER AKTIF : " << username;
    int sisaSpasi = 26 - username.length();
    for (int i = 0; i < sisaSpasi; i++) cout << " ";
    cout << "|\n +-----------------------------------------+\n\n";
}

// [ANGGOTA 5] - Penahan Tampilan Layar Menu
void tungguEnter() { cout << "\n  Tekan [Enter] untuk kembali ke Menu Utama..."; cin.get(); }

// [ANGGOTA 5] - Fungsi Pembaca Karakter Keyboard Universal
char getchUniversal() {
    #if IS_WINDOWS
        return _getch();
    #else
        char buf = 0; struct termios old = {0};
        if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
        old.c_lflag &= ~ICANON; old.c_lflag &= ~ECHO; old.c_cc[VMIN] = 1; old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0) perror("read()");
        old.c_lflag |= ICANON; old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
        return (buf);
    #endif
}

// [ANGGOTA 5] - Validasi Input Angka agar Tidak Error/Crash
int inputAngkaValid(const string& pesan, int min, int max) {
    int angka;
    while (true) {
        cout << pesan; cin >> angka;
        if (cin.fail()) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [Error] Input harus berupa angka!\n";
        } else if (angka == 0) { 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); return 0; 
        } else if (angka < min || angka > max) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [Error] Masukkan angka " << min << "-" << max << " (atau 0 untuk batal).\n";
        } else { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return angka; }
    }
}

// [ANGGOTA 5] - Validasi Input Teks Kosong
void inputTeksValid(const string& pesan, string& wadah) {
    while (true) {
        cout << pesan; getline(cin, wadah);
        if (wadah.empty()) { cout << "  [Error] Input tidak boleh kosong!\n"; } 
        else { return; }
    }
}

// [ANGGOTA 5] - Navigasi Menu Interaktif Menggunakan Tombol Panah
int bacaTombolMenu() {
    char ch = getchUniversal();
    if (ch == 27) { 
        char n1 = getchUniversal();
        if (n1 == '[') {
            char n2 = getchUniversal();
            if (n2 == 'A') return 1; if (n2 == 'B') return 2; 
        }
    }
    if (ch == 10 || ch == 13) return 3; 
    return 0;
}

// [ANGGOTA 5] - Helper Pencarian Indeks Berdasarkan Relasi ID & Pemilik
int cariIndeksTugas(int id) {
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].id == id && daftar[i].pemilik == currentUser) return i;
    }
    return -1;
}

// [ANGGOTA 5] - Helper Pencarian Akun Pengguna
int cariIndeksUser(const string& username) {
    for (int i = 0; i < jumlahUser; i++) {
        if (users[i].username == username) return i;
    }
    return -1;
}

// [ANGGOTA 5] - Auto Increment ID Tugas Baru
int dapatkanIdBaru() {
    int maxId = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].id > maxId) maxId = daftar[i].id;
    }
    return maxId + 1;
}

// [ANGGOTA 5] - Operasi Insert Linked List untuk Menyimpan Log Aktivitas
void tambahRiwayat(const string& aksi) {
    string logBaruStr = "[" + currentUser + "] " + aksi;
    NodeLog* newNode = new NodeLog;
    newNode->aktivitas = logBaruStr;
    newNode->next = nullptr;

    if (headLog == nullptr) {
        headLog = newNode;
    } else {
        NodeLog* temp = headLog;
        while (temp->next != nullptr) { temp = temp->next; }
        temp->next = newNode;
    }
}

// [ANGGOTA 5] - Deallokasi Memory Linked List (Mencegah Memory Leak)
void bersihkanRiwayat() {
    NodeLog* current = headLog;
    while (current != nullptr) {
        NodeLog* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    headLog = nullptr;
}


// ============================================================================
// PART 3: PERSISTENSI FILE HANDLING - OFSTREAM & IFSTREAM (ANGGOTA 2)
// ============================================================================

// [ANGGOTA 2] - Menyimpan Seluruh Data Tugas ke File txt
void simpanTugas() {
    ofstream file("tasks.txt");
    if (!file) return;
    for (int i = 0; i < jumlahTugas; i++) {
        file << daftar[i].id << "|" << daftar[i].namaTugas << "|" << daftar[i].kategori << "|"
             << daftar[i].prioritas << "|" << daftar[i].deadline << "|" << daftar[i].status << "|"
             << daftar[i].estimasiWaktu << "|" << daftar[i].pemilik << "\n";
    }
    file.close();
}
 
// [ANGGOTA 2] - Menyimpan Basis Akun Pengguna ke File txt
void simpanUsers() {
    ofstream file("users.txt");
    if (!file) return;
    for (int i = 0; i < jumlahUser; i++) {
        file << users[i].username << "|" << users[i].password << "\n";
    }
    file.close();
}
 
// [ANGGOTA 2] - Memuat Ulang Data Tugas dari txt ke Memori Array Dinamis
void muatTugas() {
    ifstream file("tasks.txt");
    if (!file) return;
    jumlahTugas = 0;
    string idStr, nama, kat, prioStr, dead, stat, estStr, pem;
    while (jumlahTugas < MAX_TUGAS && getline(file, idStr, '|')) {
        getline(file, nama, '|'); getline(file, kat, '|'); getline(file, prioStr, '|');
        getline(file, dead, '|'); getline(file, stat, '|'); getline(file, estStr, '|');
        getline(file, pem, '\n');
        if (idStr.empty()) continue;
        Tugas t;
        t.id = stoi(idStr); t.namaTugas = nama; t.kategori = kat; t.prioritas = stoi(prioStr);
        t.deadline = dead; t.status = stat; t.estimasiWaktu = stoi(estStr); t.pemilik = pem;
        daftar[jumlahTugas++] = t;
    }
    file.close();
}
 
// [ANGGOTA 2] - Memuat Ulang Data Akun User dari txt saat Startup
void muatUsers() {
    ifstream file("users.txt");
    if (!file) return;
    jumlahUser = 0;
    string user, pass;
    while (jumlahUser < MAX_USERS && getline(file, user, '|')) {
        getline(file, pass, '\n');
        if (user.empty()) continue;
        User u; u.username = user; u.password = pass;
        users[jumlahUser++] = u;
    }
    file.close();
}


// ============================================================================
// PART 4: STRUKTUR DATA STACK & QUEUE SIMULATION (ANGGOTA 3)
// ============================================================================

// [ANGGOTA 3] - Alokasi Penyimpanan Buffer Undo Stack
struct UndoData { Tugas data; };
UndoData undoStack[MAX_TUGAS];
int topUndo = -1;

// [ANGGOTA 3] - Operasi Push Stack Data
void pushUndo(Tugas t) {
    if (topUndo < MAX_TUGAS - 1) { undoStack[++topUndo].data = t; }
}

// [ANGGOTA 3] - Operasi Pop Stack Data
bool popUndo(Tugas &t) {
    if (topUndo == -1) return false;
    t = undoStack[topUndo--].data;
    return true;
}

// [ANGGOTA 3] - Kontrol Utama Aksi Undo User
void undoAksiTerakhir() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    Tugas t;
    cout << " +-----------------------------------------+\n";
    cout << " |             UNDO DATA CONTROLLER        |\n";
    cout << " +-----------------------------------------+\n";
    if (popUndo(t)) {
        daftar[jumlahTugas++] = t;
        simpanTugas();
        tambahRiwayat("Melakukan UNDO pengembalian tugas: " + t.namaTugas);
        cout << "  [SUCCESS] Tugas \"" << t.namaTugas << "\" berhasil dikembalikan.\n";
    } else { cout << "  [Info] Stack kosong, tidak ada riwayat yang bisa di-undo.\n"; }
    tungguEnter();
}

// [ANGGOTA 3] - Alokasi Penyimpanan Antrean Kerja Queue
struct QueueTugas { Tugas data; };
QueueTugas antrean[MAX_TUGAS];
int frontQueue = 0;
int rearQueue = -1;

// [ANGGOTA 3] - Operasi Enqueue (Masuk Antrean)
void enqueue(Tugas t) {
    if (rearQueue < MAX_TUGAS - 1) { antrean[++rearQueue].data = t; }
}

// [ANGGOTA 3] - Operasi Dequeue (Keluar Antrean / Di-proses)
bool dequeue(Tugas &t) {
    if (frontQueue > rearQueue) return false;
    t = antrean[frontQueue++].data;
    return true;
}

// [ANGGOTA 3] - Penyusunan Antrean Prioritas (Mendahulukan Tugas Penting)
void buatAntreanPrioritas() {
    frontQueue = 0; rearQueue = -1;
    for (int p = 1; p <= 3; p++) {
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik == currentUser && daftar[i].prioritas == p && daftar[i].status != "Selesai") {
                enqueue(daftar[i]);
            }
        }
    }
    cout << "  [Queue] Buffer antrean prioritas sukses dikonfigurasi!\n";
}

// [ANGGOTA 3] - Menu Kontrol Simulasi Antrean Kerja (Queue)
void prosesAntrean() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |           MANAJEMEN QUEUE TUGAS         |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Bangun Antrean Berdasarkan Prioritas|\n";
    cout << " | [2] Eksekusi/Proses Antrean Terdepan    |\n";
    cout << " | [0] Kembali ke Menu Utama               |\n";
    cout << " +-----------------------------------------+\n";
    int opsi = inputAngkaValid("  Pilih Opsi (0-2): ", 0, 2);
    if (opsi == 0) return;
    if (opsi == 1) {
        buatAntreanPrioritas();
    } else {
        Tugas t;
        if (dequeue(t)) {
            tambahRiwayat("Memproses antrean terdepan task: " + t.namaTugas);
            cout << "  [Processing] ID:" << t.id << " -> " << t.namaTugas << " sedang dikerjakan.\n";
        } else { cout << "  [Info] Buffer Queue kosong, tidak ada antrean.\n"; }
    }
    tungguEnter();
}


// ============================================================================
// PART 5: FITUR UTAMA OPERASI CRUD ARRAY & STRUCT (ANGGOTA 1)
// ============================================================================

// [ANGGOTA 1] - Fitur Create (Tambah Tugas Baru)
void tambahTugas() {
    if (jumlahTugas >= MAX_TUGAS) {
        tampilkanHeaderKotak();
        cout << "  [Gagal] Memori penuh!\n";
        tungguEnter();
        return;
    }
 
    Tugas tugasBaru;
    tugasBaru.id = dapatkanIdBaru();
    tugasBaru.pemilik = currentUser;
 
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |             TAMBAH TUGAS BARU           |\n";
    cout << " +-----------------------------------------+\n";
    cout << "  ID Tugas Otomatis : " << tugasBaru.id << "\n";
    cout << "  *Ketik 'batal' kapan saja untuk keluar\n";
    cout << "  -------------------------------------------\n";
    
    inputTeksValid("  Masukkan Nama Tugas: ", tugasBaru.namaTugas);
    if (tugasBaru.namaTugas == "batal" || tugasBaru.namaTugas == "0") {
        cout << "\n  [Info] Pengisian tugas dibatalkan.\n";
        tungguEnter(); return;
    }
 
    string listKategori[] = {"Kuliah", "Pekerjaan", "Umum"};
    int indeksKategori = 0; 
    bool selesaiKategori = false;

    while (!selesaiKategori) {
        tampilkanHeaderKotak();
        tampilkanUserActive(currentUser);
        cout << " +-----------------------------------------+\n";
        cout << " |             TAMBAH TUGAS BARU           |\n";
        cout << " +-----------------------------------------+\n";
        cout << "  Nama Tugas: " << tugasBaru.namaTugas << "\n";
        cout << "  -------------------------------------------\n";
        cout << "  Pilih Kategori (Gunakan Panah Atas/Bawah -> Enter):\n";
        cout << "  +------------------------------+\n";

        for (int i = 0; i < 3; i++) {
            if (i == indeksKategori) {
                cout << "  |  \033[7m  " << listKategori[i] << "  \033[0m";
                int sisa = 23 - listKategori[i].length();
                for(int j=0; j<sisa; j++) cout << " ";
                cout << "|\n";
            } else {
                cout << "  |     " << listKategori[i];
                int sisa = 25 - listKategori[i].length();
                for(int j=0; j<sisa; j++) cout << " ";
                cout << "|\n";
            }
        }
        cout << "  +------------------------------+\n";

        int tombol = bacaTombolMenu();
        if (tombol == 1) { indeksKategori = (indeksKategori - 1 + 3) % 3; } 
        else if (tombol == 2) { indeksKategori = (indeksKategori + 1) % 3; } 
        else if (tombol == 3) { tugasBaru.kategori = listKategori[indeksKategori]; selesaiKategori = true; }
    }

    string listPrioritas[] = {"Tinggi", "Sedang", "Rendah"};
    int indeksPrio = 1; 
    bool selesaiPrio = false;

    while (!selesaiPrio) {
        tampilkanHeaderKotak();
        tampilkanUserActive(currentUser);
        cout << " +-----------------------------------------+\n";
        cout << " |             TAMBAH TUGAS BARU           |\n";
        cout << " +-----------------------------------------+\n";
        cout << "  Nama Tugas: " << tugasBaru.namaTugas << "\n";
        cout << "  Kategori  : " << tugasBaru.kategori << "\n";
        cout << "  -------------------------------------------\n";
        cout << "  Pilih Prioritas (Gunakan Panah Atas/Bawah -> Enter):\n";
        cout << "  +------------------------------+\n";

        for (int i = 0; i < 3; i++) {
            if (i == indeksPrio) {
                cout << "  |  \033[7m  " << listPrioritas[i] << "  \033[0m";
                int sisa = 23 - listPrioritas[i].length();
                for(int j=0; j<sisa; j++) cout << " ";
                cout << "|\n";
            } else {
                cout << "  |     " << listPrioritas[i];
                int sisa = 25 - listPrioritas[i].length();
                for(int j=0; j<sisa; j++) cout << " ";
                cout << "|\n";
            }
        }
        cout << "  +------------------------------+\n";

        int tombol = bacaTombolMenu();
        if (tombol == 1) { indeksPrio = (indeksPrio - 1 + 3) % 3; } 
        else if (tombol == 2) { indeksPrio = (indeksPrio + 1) % 3; } 
        else if (tombol == 3) { tugasBaru.prioritas = indeksPrio + 1; selesaiPrio = true; }
    }

    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |             TAMBAH TUGAS BARU           |\n";
    cout << " +-----------------------------------------+\n";
    cout << "  Nama Tugas: " << tugasBaru.namaTugas << "\n";
    cout << "  Kategori  : " << tugasBaru.kategori << "\n";
    cout << "  Prioritas : " << listPrioritas[tugasBaru.prioritas - 1] << "\n";
    cout << "  -------------------------------------------\n";

    cout << "  Input Deadline Terstruktur:\n";
    int tgl = inputAngkaValid("     - Masukkan Tanggal (1-31, 0:Batal): ", 1, 31); if (tgl == 0) return;
    int bln = inputAngkaValid("     - Masukkan Bulan (1-12, 0:Batal)  : ", 1, 12); if (bln == 0) return;
    int thn = inputAngkaValid("     - Masukkan Tahun (Min 2026,0:Batal): ", 2026, 2100); if (thn == 0) return;

    string strTgl = (tgl < 10) ? "0" + to_string(tgl) : to_string(tgl);
    string strBln = (bln < 10) ? "0" + to_string(bln) : to_string(bln);
    tugasBaru.deadline = to_string(thn) + "-" + strBln + "-" + strTgl;

    int estimasi = inputAngkaValid("  Estimasi Waktu (dalam menit, 0:Batal): ", 1, 1440); if (estimasi == 0) return;
    tugasBaru.estimasiWaktu = estimasi;
    
    tugasBaru.status = "Belum Selesai";
    daftar[jumlahTugas++] = tugasBaru;
    
    simpanTugas(); // Memanggil Fungsi Anggota 2
    tambahRiwayat("Menambahkan tugas baru: " + tugasBaru.namaTugas); // Memanggil Fungsi Anggota 5
    cout << "\n  [Sukses] Tugas \"" << tugasBaru.namaTugas << "\" berhasil disimpan!\n";
    tungguEnter();
}

// [ANGGOTA 1] - Fitur Read (Tampilkan & Saring Data Tugas)
void tampilkanTugas() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |             FILTER DATA TUGAS           |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Tampilkan Semua Tugas               |\n";
    cout << " | [2] Kategori: Kuliah                    |\n";
    cout << " | [3] Kategori: Pekerjaan                 |\n";
    cout << " | [4] Kategori: Umum                      |\n";
    cout << " | [0] Batal dan Kembali                   |\n";
    cout << " +-----------------------------------------+\n";
    
    int pilihanFilter = inputAngkaValid("  Pilih opsi filter (0-4): ", 0, 4);
    if (pilihanFilter == 0) return;
 
    string filterKategori = "";
    if (pilihanFilter == 2) filterKategori = "Kuliah";
    else if (pilihanFilter == 3) filterKategori = "Pekerjaan";
    else if (pilihanFilter == 4) filterKategori = "Umum";
 
    int totalTugasTercetak = 0;
    cout << "\n  --- DAFTAR TUGAS ANDA ---\n";
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik != currentUser) continue;
        if (pilihanFilter != 1 && daftar[i].kategori != filterKategori) continue;
 
        totalTugasTercetak++;
        cout << "  +--------------------------------------------------------+\n";
        cout << "    ID Tugas   : " << daftar[i].id << "\n";
        cout << "    Nama Tugas : " << daftar[i].namaTugas << "\n";
        cout << "    Kategori   : " << daftar[i].kategori << "\n";
        cout << "    Prioritas  : " << (daftar[i].prioritas == 1 ? "Tinggi" : (daftar[i].prioritas == 2 ? "Sedang" : "Rendah")) << "\n";
        cout << "    Deadline   : " << daftar[i].deadline << "\n";
        cout << "    Estimasi   : " << daftar[i].estimasiWaktu << " Menit\n";
        cout << "    Status     : [" << daftar[i].status << "]\n";
        cout << "  +--------------------------------------------------------+\n";
    }
    if (totalTugasTercetak == 0) cout << "  [Info] Tidak ditemukan data tugas yang cocok.\n";
    tungguEnter();
}
 
// [ANGGOTA 1] - Fitur Update (Ubah Status / Edit Detail Isi Tugas)
void updateTugas() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |             UPDATE DATA TUGAS           |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Tandai Selesai                      |\n";
    cout << " | [2] Edit Detail Tugas Total             |\n";
    cout << " | [3] Search & Replace Kata Kunci         |\n";
    cout << " | [0] Batal dan Kembali                   |\n";
    cout << " +-----------------------------------------+\n";
    int pilihanUpdate = inputAngkaValid("  Pilih opsi (0-3): ", 0, 3);
    if (pilihanUpdate == 0) return;
    if (pilihanUpdate == 1) {
        int cariID = inputAngkaValid("  Masukkan ID tugas yang selesai (0:Batal): ", 0, 100000);
        if (cariID == 0) return;
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) { cout << "  [Gagal] ID Tugas tidak valid.\n"; tungguEnter(); return; }
        
        daftar[indeks].status = "Selesai";
        simpanTugas();
        tambahRiwayat("Menandai selesai tugas ID: " + to_string(cariID));
        cout << "  [Sukses] Tugas berhasil di-update menjadi Selesai!\n";
    } else if (pilihanUpdate == 2) {
        int cariID = inputAngkaValid("  Masukkan ID tugas yang mau diedit (0:Batal): ", 0, 100000);
        if (cariID == 0) return;
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) { cout << "  [Gagal] ID data tidak ditemukan.\n"; tungguEnter(); return; }
 
        inputTeksValid("  Nama Tugas Baru (atau 'batal'): ", daftar[indeks].namaTugas);
        if (daftar[indeks].namaTugas == "batal") return;

        cout << "  Kategori Baru [1:Kuliah, 2:Pekerjaan, 3:Umum, 0:Batal]: ";
        char ch = getchUniversal(); cout << ch << "\n";
        if (ch == '1') daftar[indeks].kategori = "Kuliah";
        else if (ch == '2') daftar[indeks].kategori = "Pekerjaan";
        else if (ch == '3') daftar[indeks].kategori = "Umum";

        simpanTugas();
        tambahRiwayat("Mengedit detail tugas ID: " + to_string(cariID));
        cout << "  [Sukses] Detail data tugas berhasil dirombak!\n";
    } else {
        string kataKunci, teksBaru;
        inputTeksValid("  Masukkan kata kunci yang dicari (atau 'batal'): ", kataKunci);
        if (kataKunci == "batal") return;
        inputTeksValid("  Masukkan kalimat penggantinya: ", teksBaru);
        
        int jumlahTerganti = 0;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik == currentUser && daftar[i].namaTugas.find(kataKunci) != string::npos) {
                daftar[i].namaTugas = teksBaru;
                jumlahTerganti++;
            }
        }
        if (jumlahTerganti > 0) { 
            simpanTugas();
            tambahRiwayat("Search & Replace nama tugas kata kunci: " + kataKunci);
            cout << "  [Sukses] Berhasil memperbarui " << jumlahTerganti << " tugas.\n";
        } else { cout << "  [Info] Kata kunci tidak cocok.\n"; }
    }
    tungguEnter();
}
 
// [ANGGOTA 1] - Fitur Delete (Hapus Tugas Tunggal / Pembersihan Masal)
void hapusTugas() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |              PEMBERSIHAN DATA            |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Hapus Satu Tugas (Spesifik ID)      |\n";
    cout << " | [2] Bersihkan Semua Tugas Selesai       |\n";
    cout << " | [0] Batal dan Kembali                   |\n";
    cout << " +-----------------------------------------+\n";
    int pilihanHapus = inputAngkaValid("  Pilih opsi (0-2): ", 0, 2);
    if (pilihanHapus == 0) return;
    if (pilihanHapus == 1) {
        int cariID = inputAngkaValid("  Masukkan ID tugas yang mau dihapus (0:Batal): ", 0, 100000);
        if (cariID == 0) return;
        int indeks = cariIndeksTugas(cariID);
        if (indeks == -1) { cout << "  [Gagal] ID data salah.\n"; tungguEnter(); return; }
        
        pushUndo(daftar[indeks]); // Menyimpan ke Buffer Stack Anggota 3 sebelum dihapus
        for (int i = indeks; i < jumlahTugas - 1; i++) { daftar[i] = daftar[i + 1]; }
        jumlahTugas--;
        simpanTugas();
        tambahRiwayat("Menghapus tugas ID: " + to_string(cariID));
        cout << "  [Sukses] Tugas berhasil dihapus!\n";
    } else {
        int jumlahTerhapus = 0;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik == currentUser && daftar[i].status == "Selesai") {
                for (int j = i; j < jumlahTugas - 1; j++) { daftar[j] = daftar[j + 1]; }
                jumlahTugas--; jumlahTerhapus++; i--;
            }
        }
        if (jumlahTerhapus > 0) { 
            simpanTugas();
            tambahRiwayat("Membersihkan semua tugas berstatus Selesai");
            cout << "  [Sukses] " << jumlahTerhapus << " tugas selesai dibersihkan.\n";
        } else { cout << "  [Info] Tidak ada data tugas 'Selesai'.\n"; }
    }
    tungguEnter();
}


// ============================================================================
// PART 6: METODE ALGORITMA SEARCHING & SORTING DATA (ANGGOTA 4)
// ============================================================================

// [ANGGOTA 4] - Implementasi Linear/Sequential Search Berdasarkan ID & Nama
void cariTugas() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |               PENCARIAN DATA            |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Cari Via ID Tugas                   |\n";
    cout << " | [2] Cari Via Kata Kunci Nama            |\n";
    cout << " | [0] Batal dan Kembali                   |\n";
    cout << " +-----------------------------------------+\n";
    int opsi = inputAngkaValid("  Pilih Opsi Cari (0-2): ", 0, 2);
    if (opsi == 0) return;

    bool ditemukan = false;
    if (opsi == 1) {
        int cariID = inputAngkaValid("  Masukkan ID yang dicari (0:Batal): ", 0, 100000);
        if (cariID == 0) return;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].id == cariID && daftar[i].pemilik == currentUser) {
                cout << "\n  [DATA DITEMUKAN]\n  -----------------------------------\n";
                cout << "  Nama Task : " << daftar[i].namaTugas << " [" << daftar[i].status << "]\n";
                ditemukan = true; break;
            }
        }
    } else {
        string cariNama;
        inputTeksValid("  Masukkan kata kunci pencarian (atau 'batal'): ", cariNama);
        if (cariNama == "batal") return;
        for (int i = 0; i < jumlahTugas; i++) {
            if (daftar[i].pemilik == currentUser && daftar[i].namaTugas.find(cariNama) != string::npos) {
                cout << "  [MATCH]: ID (" << daftar[i].id << ") - " << daftar[i].namaTugas << "\n";
                ditemukan = true;
            }
        }
    }
    if (!ditemukan) cout << "  [Info] Data tidak ditemukan.\n";
    tungguEnter();
}

// [ANGGOTA 4] - Implementasi Bubble Sort Mengurutkan Prioritas & Durasi Estimasi
void urutkanTugas() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    cout << " +-----------------------------------------+\n";
    cout << " |         SORTING ALGORITMA UTAMA         |\n";
    cout << " +-----------------------------------------+\n";
    cout << " | [1] Berdasarkan Urutan Prioritas       |\n";
    cout << " | [2] Berdasarkan Durasi Waktu Tercepat   |\n";
    cout << " | [0] Batal dan Kembali                   |\n";
    cout << " +-----------------------------------------+\n";
    int opsi = inputAngkaValid("  Pilih Opsi (0-2): ", 0, 2);
    if (opsi == 0) return;
    for (int i = 0; i < jumlahTugas - 1; i++) {
        for (int j = 0; j < jumlahTugas - i - 1; j++) {
            bool harusTukar = (opsi == 1) ?
                (daftar[j].prioritas > daftar[j+1].prioritas) : (daftar[j].estimasiWaktu > daftar[j+1].estimasiWaktu);
            if (harusTukar) { Tugas temp = daftar[j]; daftar[j] = daftar[j+1]; daftar[j+1] = temp; }
        }
    }
    simpanTugas();
    tambahRiwayat("Mengurutkan struktur data array utama");
    cout << "  [Sukses] Struktur data array berhasil diurutkan!\n";
    tungguEnter();
}


// ========================================================
// INTEGRASI BONUS SYSTEM - LOGIN & STATISTIK (ANGGOTA 5)
// ========================================================

// [ANGGOTA 5] - Menampilkan Log Linked List & Akumulasi Statistik Kinerja
void statistikDanLog() {
    tampilkanHeaderKotak();
    tampilkanUserActive(currentUser);
    int total = 0, selesai = 0, totalEstimasi = 0;
    for (int i = 0; i < jumlahTugas; i++) {
        if (daftar[i].pemilik != currentUser) continue;
        total++;
        if (daftar[i].status == "Selesai") selesai++;
        totalEstimasi += daftar[i].estimasiWaktu;
    }
 
    cout << " +-----------------------------------------+\n";
    cout << " |          DATA METRIK PRODUKTIVITAS      |\n";
    cout << " +-----------------------------------------+\n";
    cout << "   » Total Tugas     : " << total << "\n";
    cout << "   » Berhasil Selesai: " << selesai << "\n";
    cout << "   » Sisa Estimasi   : " << (totalEstimasi / 60.0) << " Jam\n";
    cout << " +-----------------------------------------+\n";

    cout << "\n +-----------------------------------------+\n";
    cout << " |       LOG AKTIVITAS USER (LINKED LIST)  |\n";
    cout << " +-----------------------------------------+\n";
    
    int counter = 0;
    string searchKey = "[" + currentUser + "]";
    NodeLog* currNode = headLog; // Penelusuran (Traversasi) Node Linked List
    
    while (currNode != nullptr) {
        if (currNode->aktivitas.find(searchKey) != string::npos) {
            cout << "   [" << ++counter << "] " << currNode->aktivitas << "\n";
        }
        currNode = currNode->next;
    }
    if (counter == 0) cout << "   [Info] Log jejak digital linked list masih kosong.\n";
    tungguEnter();
}

// [ANGGOTA 5] - Sistem Validasi Menu Otentikasi Login Akun Multi-User
bool loginMenu() {
    if (jumlahUser == 0) {
        users[jumlahUser].username = "admin"; users[jumlahUser++].password = "admin";
        users[jumlahUser].username = "user"; users[jumlahUser++].password = "user";
        simpanUsers();
    }
    while (true) {
        tampilkanHeaderKotak(); 
        cout << " +-----------------------------------------+\n";
        cout << " |         SISTEM OTENTIKASI LOGIN         |\n";
        cout << " +-----------------------------------------+\n\n";
        cout << "  [1] Login Akun\n  [2] Buat Akun Baru\n  [3] Tutup Program\n";
        cout << "  -------------------------------------------\n";
        
        int pilihan = inputAngkaValid(" Pilih Opsi (1-3): ", 1, 3);
        if (pilihan == 1) {
            string username, password;
            cout << "\n --- Form Masuk Akun ---\n";
            inputTeksValid(" Username: ", username); inputTeksValid(" Password: ", password);
            int indeks = cariIndeksUser(username);
            if (indeks != -1 && users[indeks].password == password) {
                currentUser = username;
                cout << "\n [Sukses] Selamat datang kembali, " << currentUser << "!\n";
                cout << " Tekan Enter untuk masuk ke Dashboard..."; cin.get(); return true;
            }
            cout << " [Error] Username atau sandi keliru.\n\n Tekan Enter untuk mencoba lagi..."; cin.get();
        } else if (pilihan == 2) {
            if (jumlahUser >= MAX_USERS) { cout << " [Error] Memori kuota user penuh.\n"; cin.get(); continue; }
            string username;
            cout << "\n --- Form Daftar Akun ---\n";
            inputTeksValid(" Daftar Username Baru: ", username);
            if (cariIndeksUser(username) != -1) {
                cout << " [Error] Nama user tersebut sudah terpakai.\n\n Tekan Enter untuk mencoba lagi...";
                cin.get(); continue;
            }
            string password; inputTeksValid(" Daftar Password Baru: ", password);
            users[jumlahUser].username = username; users[jumlahUser].password = password; jumlahUser++;
            simpanUsers();
            cout << " [Sukses] Akun \"" << username << "\" berhasil dibuat!\n\n Tekan Enter untuk kembali..."; cin.get();
        } else { return false; }
    }
}


// =======================================
// SISTEM MENU DAN KONTROL UTAMA PROGRAM
// =======================================

int main() {
    muatUsers(); // Memanggil Fungsi Anggota 2
    muatTugas(); // Memanggil Fungsi Anggota 2
    if (!loginMenu()) { 
        delete[] daftar; // Pembersihan Array Dinamis Anggota 1
        bersihkanRiwayat(); // Pembersihan Linked List Anggota 5
        cout << "\nProgram ditutup. Sampai jumpa!\n"; 
        return 0; 
    }

    while (true) {
        tampilkanHeaderKotak();
        tampilkanUserActive(currentUser);
        cout << " 1. Tambah Tugas\n";
        cout << " 2. Tampilkan Tugas & Filter\n";
        cout << " 3. Update / Edit Tugas\n";
        cout << " 4. Hapus Tugas (Delete)\n";
        cout << " 5. Cari Tugas\n";
        cout << " 6. Urutkan Tugas\n";
        cout << " 7. Undo Aksi Terakhir\n";
        cout << " 8. Siapkan Antrean & Proses\n";
        cout << " 9. Statistik & Log Riwayat Aktivitas\n";
        cout << " 10. Logout & Ganti Akun\n\n";
        
        int menu = inputAngkaValid(" Pilih Opsi Menu (1-10): ", 1, 10);
        switch (menu) {
            case 1: tambahTugas(); break;     // Fitur Anggota 1
            case 2: tampilkanTugas(); break;  // Fitur Anggota 1
            case 3: updateTugas(); break;     // Fitur Anggota 1
            case 4: hapusTugas(); break;      // Fitur Anggota 1
            case 5: cariTugas(); break;       // Fitur Anggota 4
            case 6: urutkanTugas(); break;    // Fitur Anggota 4
            case 7: undoAksiTerakhir(); break; // Fitur Anggota 3
            case 8: prosesAntrean(); break;    // Fitur Anggota 3
            case 9: statistikDanLog(); break;  // Fitur Anggota 5
            case 10:
                cout << "\nKeluar dari akun " << currentUser << "...\n";
                if (!loginMenu()) {
                    delete[] daftar;
                    bersihkanRiwayat();
                    return 0;
                }
                break;
        }
    } 
    
    // Pembersihan akhir saat program ditutup normal
    delete[] daftar;
    bersihkanRiwayat();
    return 0;
}