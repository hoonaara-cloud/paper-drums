# دریافت فایل VST3 ویندوزی از GitHub

این پروژه به GitHub Actions مجهز شده است. GitHub بعد از هر Push، پروژه را روی Windows کامپایل می‌کند.

## روش پیشنهادی

1. فایل ZIP را روی کامپیوترت Extract کن.
2. در GitHub یک Repository جدید بساز.
3. **محتویات Extract شده** را داخل Repository آپلود کن؛ خود فایل ZIP را به‌تنهایی در GitHub آپلود نکن، چون GitHub فایل‌های داخل ZIP را به‌عنوان Workflow نمی‌خواند.
4. بعد از Push، در صفحه‌ی Repository وارد تب **Actions** شو.
5. Workflow با نام **Build Paper Drums VST3 for Windows** را باز کن.
6. وقتی وضعیت سبز شد، از بخش **Artifacts** فایل `PaperDrums-VST3-Windows` را دانلود کن.
7. ZIP دانلودشده را Extract کن و فایل `Paper Drums.vst3` را در این مسیر Windows قرار بده:

```text
C:\Program Files\Common Files\VST3
```

8. DAW را باز کن و Plugin Scan/Rescan را اجرا کن.

## ساخت Release قابل دانلود

برای این‌که فایل VST3 در بخش **Releases** هم قرار بگیرد، در Git یک Tag بساز و Push کن:

```bash
git tag v0.1.0
git push origin v0.1.0
```

Workflow به‌صورت خودکار یک Release می‌سازد و فایل `PaperDrums-VST3-Windows.zip` را به آن پیوست می‌کند.

## اجرای دستی

در تب **Actions** می‌توانی Workflow را انتخاب کنی و از گزینه‌ی **Run workflow** آن را دستی اجرا کنی.

## نکته

اولین Build ممکن است چند دقیقه طول بکشد، چون JUCE از GitHub دریافت می‌شود. این طبیعی است.
