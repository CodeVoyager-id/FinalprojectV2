# 📝 Project To-Do App: Catatan Tambahan buat Tugas Kelompok


### 1. Anggota 2 (File Handling)
* **Yang perlu di tambahin:** Bikin sistem biar data tugas sama data akun user gak hilang pas aplikasi ditutup. kita harus bikin fungsi buat otomatis **menyimpan** semua isi data ke file teks (`tasks.txt` dan `users.txt`) tiap kali ada tugas baru atau akun baru dibuat. Terus bikin juga fungsi buat **memuat (load)** kembali file teks itu pas aplikasi pertama kali dibuka biar datanya muncul lagi.

### 2. Anggota 3 (Stack & Queue)
* **Yang perlu di tambahin:**
  * **Fitur Undo (Stack):** Bikin sistem biar user bisa batalin aksi terakhir mereka (misalnya pas gak sengaja hapus tugas atau salah ngedit). Jadi sebelum data diubah/dihapus, datanya di amankan dulu ke dalam Stack biar bisa di-Undo.
  * **Fitur Antrean (Queue):** Bikin sistem antrean tugas berdasarkan prioritas atau yang paling mendesak, jadi nanti tugas-tugas itu bisa diproses atau diselesaikan satu per satu sesuai urutan antrean.

### 3. Anggota 4 (Searching & Sorting)
* **Yang perlu di tambahin:**
  * **Fitur Cari (Searching):** Bikin menu baru biar user bisa nyari tugas secara spesifik, entah dicari berdasarkan **ID Tugas**-nya atau langsung ketik **Nama Tugas**-nya.
  * **Fitur Urutkan (Sorting):** Urutin daftar tugas pas ditampilin ke layar. bisa kasih opsi ke user mau diurutin berdasarkan **Tingkat Prioritas** (dari yang paling tinggi ke rendah) atau berdasarkan **Estimasi Waktu** (dari yang paling cepat selesai ke yang paling lama). 

### 4. Anggota 5 (Linked List & Integrasi)
* **Yang perlu di tambahin:**
  * **Fitur Riwayat (Linked List):** Di kode yang sekarang, fitur log riwayat aktivitas user masih pakai array biasa. Tugas anggota 5 adalah ngubah logika riwayat itu biar pakai Linked List, biar penyimpanannya dinamis dan gak dibatasi kuota array lagi.
  * **Integrasi Kode:** anggota 5 bertanggung jawab buat nggabungin semua codingan dari Anggota 1 sampai 4 biar bisa jalan bareng di menu utama tanpa ada error atau variabel yang tabrakan.

---

## ⚠️ Aturan Main
1. **Gak boleh pakai library instan (STL):** Kayak `<vector>`, `<stack>`, `<queue>`, atau fungsi `std::sort`. Semuanya (Stack, Queue, Linked List, Sorting, Searching) wajib kita bikin manual dari nol pakai array atau pointer biasa.
2. Kasih komentar atau tanda nama di masing masing code tugas masing masing anggota biar mudah untuk mencari bagiannya masing-masing.

## Readme sementara