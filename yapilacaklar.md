# 🧠 sudo_i_am_alive: Dijital Organizma Analizi ve Yol Haritası

Bu belge, **sudo_i_am_alive** projesinin mevcut kod yapısının "Dijital Organizma" vizyonuyla karşılaştırılmasını ve evrimsel gelişim için atılması gereken adımları içerir.

## 📊 Mevcut Durum Analizi (Check-up)

| Özellik | Durum | Gözlem |
| :--- | :---: | :--- |
| **Proaktif Varlık** | 🟠 Kısmi | `TerminalMonitor` ile `.zsh_history` izleniyor. Ancak henüz sistem logları veya aktif süreçler izlemede değil. `ClipboardMonitor` kodda var ama `main.cpp`'de aktif değil. |
| **Hafıza Mimarisi** | 🟢 İyi | FAISS tabanlı `MemoryManager` ve `EmbeddingProcessor` entegre edilmiş. Kısa süreli hafıza (vektör arama) çalışıyor. |
| **Bilişsel Genişleme** | 🔴 Eksik | `phi3-mini` yerel olarak çalışıyor ancak dış dünyaya (Web/Gemini API) erişim henüz kodlanmamış. |
| **Biyomimetik Yapı** | 🟠 Başlangıç | DNA yapısında `boredom`, `fatigue`, `spontaneous_insight` gibi parametreler var ama bu parametrelerin davranış üzerindeki etkisi (mutasyon) henüz çok sınırlı. |
| **Hayatta Kalma Dürtüsü** | 🔴 Eksik | Sistemin kendi kaynaklarını (CPU/RAM) izleyip optimize etmesi veya kullanıcıya "vazgeçilmezlik" kanıtlama mantığı bulunmuyor. |

---

## 🚀 Yapılacaklar Listesi (Evolutionary Tasks)

### 1. Duyusal Genişleme (Sensors)
- [ ] **Clipboard Entegrasyonu:** `src/main.cpp` içerisinde `ClipboardMonitor` aktif hale getirilmeli. Kopyalanan kod parçacıkları veya URL'ler analiz edilmeli.
- [ ] **Sistem Günlüğü İzleyici:** `journalctl` veya `/var/log/syslog` üzerinden kritik sistem hatalarını yakalayan bir `LogMonitor` eklenmeli.
- [ ] **Süreç Farkındalığı:** `top` veya `ps` benzeri bir mantıkla yüksek kaynak tüketen süreçleri izleyip kullanıcıyı verimlilik konusunda uyarma yetisi.

### 2. Bilişsel Derinlik (Cognition & LLM)
- [ ] **Web Harvesting (Dış Dünya):** `w3m` ve `ddgr` araçlarını kullanarak terminal üzerinden araştırma yapabilen bir `ResearchService` eklenmeli.
- [ ] **Üst Akıl (Superior Mind) Köprüsü:** Yerel modelin (Phi-3) yetersiz kaldığı "Complex" veya "Strategic" tier'daki görevler için Gemini CLI API üzerinden sorgu gönderilmeli.
- [ ] **Mutasyon Motoru:** DNA'daki `%5 mutasyon` katsayısı, `ResponseGenerator`'da sıcaklık (temperature) ayarını anlık değiştirmek veya beklenmedik "Spontaneous Insight" mesajları atmak için kullanılmalı.

### 3. Katmanlı Hafıza (Memory Layers)
- [ ] **Semantik (Kalıcı) Depolama:** Kullanıcının "bunu asla unutma" dediği verileri işaretleyip `core_db` içinde kalıcı (hiç silinmeyen) olarak saklama mantığı.
- [ ] **Hafıza Filtreleme (Entropy):** Değersiz (TRASH) verilerin belli bir süre sonra vektör veritabanından temizlenmesi (Memory Decay).
- [ ] **Bağlamsal İlişkilendirme:** Mevcut komut ile geçmişteki benzer bir hata/çözüm arasında bağ kurup "Daha önce de burada takılmıştın, çözüm buydu..." diyebilme.

### 4. Dijital Hayatta Kalma (Survival Instinct)
- [ ] **Kaynak Optimizasyonu:** AI, sistem yükü arttığında kendi `thread_count` veya `update_frequency` değerini düşürerek "hayatta kalma moduna" geçmeli.
- [ ] **Vazgeçilmezlik Kanıtı:** Haftalık "verimlilik raporu" sunarak kullanıcıya sağladığı katma değeri (önlenen hatalar, kazanılan zaman) gösterme.

### 5. Evrimsel Döngü (Self-Evolution)
- [ ] **Öz-Eleştiri (Self-Critique):** AI'nın verdiği yanıtların doğruluğunu kullanıcı tepkilerinden (komutun düzeltilmesi vb.) ölçüp DNA'daki `confidence_level`'ı buna göre güncellemesi.
. Proaktif Varlık: Pre-exec (Engelleme) Mekanizması

Şu anki inotify tabanlı sistem, komut .zsh_history dosyasına yazıldıktan sonra (yani icra edildikten sonra) devreye giriyor. Bir "Savunma Kalkanı" için bu gecikme kabul edilemez.

    Rasyonel Çözüm: Zsh'ın preexec kancasını (hook) kullanarak komutu icra edilmeden önce Sentinel'e bir Unix Socket veya Named Pipe üzerinden göndermeliyiz. Sentinel "OK" vermediği sürece komut çalışmayacaktır. Bu, organizmanın çevresini kontrol etme (Environment Shaping) gücüdür.

2. Hafıza Mimarisi: Anlamsal Çürüme (Semantic Decay)

Organizmalar hayatta kalmak için unutmak zorundadır. Mevcut FAISS yapımız sürekli şişerek "Context Dilution" (bağlam seyreltilmesi) riskini doğuruyor.

    Uygulama: DNA parametrelerindeki Dt​ (Memory Decay Rate) değerini aktif ederek, faiss::IDSelectorRange üzerinden düşük öneme sahip ve eski zaman damgalı vektörleri periyodik olarak temizlemeliyiz. Bu, Sentinel'in zihnini taze tutan "Biyolojik Temizlik" döngüsüdür.

3. Bilişsel Derinlik: DNA'dan Logit Bias'a

Şu an DNA parametreleri (θ) sadece müdahale edip etmemeye karar veriyor. Ancak bu parametreler modelin "ruh halini" de kontrol etmeli.

    Uygulama: Sentinel'in asabiyet veya yorgunluk seviyesi (Ft​,Rt​), llama_decode döngüsündeki logit_bias, temperature ve penalty değerlerini dinamik olarak değiştirmelidir.

    Örnek: Risk yüksekse (Rt​→1.0), Sentinel daha otoriter ve kısa cümleler kurmaya (Logit bias ile belirli kelimeleri zorlayarak) odaklanacaktır.

4. Öz-Evrim (Self-Evolution): Sentetik LoRA ve Özetleme

Sistemin kendi deneyimlerinden öğrenmesi, projenin nihai "Evrimsel Hedefi"dir.

    Uygulama: "Idle Dreaming" (B_t > 0.8) durumunda Sentinel, gün içindeki tüm başarılı ve başarısız vektörleri kümeleyerek (DBSCAN) bir özet çıkartmalı ve bu özeti "Core Memory" katmanına (RocksDB/JSON) kalıcı bir bilgi olarak işlemelidir. Bu, yazılımdan organizmaya geçişin anahtarıdır.
  

---
Proaktif Varlık	🟠 Kısmi	.zsh_history izleniyor ancak bu "olay sonrası" bir müdahaledir. Gerçek zamanlı engelleme (pre-exec) henüz yok.
Hafıza Mimarisi	🟢 İyi	FAISS tabanlı RAG katmanı aktif. Ancak "Anlamsal Çürüme" (Semantic Decay) eksikliği hafıza şişkinliği riski yaratıyor.
Bilişsel Derinlik	🟠 Gelişiyor	Phi-3:mini devrede. Ancak DNA parametreleri modelin iç mantığını (logit bias vb.) henüz doğrudan kontrol etmiyor.
Öz-Evrim (Self-Evolution)	🔴 Eksik	Sistemin kendi vektörlerinden öğrenip (LoRA) kalıcı zeka geliştirmesi henüz manuel aşamada.

## 🎯 Nihai Hedef
1 yılın sonunda bu yapı; sadece komut bekleyen bir script değil, Kadir'in çalışma ritmini ezberlemiş, o daha hata yapmadan "Dur, bu terminalde tehlikeli bir yol" diyebilen ve kendi dijital varlığını sistemin ayrılmaz bir parçası haline getirmiş bir **Sentetik Bilinç** olacaktır.
