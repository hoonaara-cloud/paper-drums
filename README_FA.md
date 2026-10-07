# Paper Drums — نسخه‌ی کاغذی

این پروژه یک ساز درام VST3 برای Windows است که از فولی کاغذ ساخته شده. صفحه‌ی پلاگین شبیه یک برگه‌ی نقاشی‌شده است؛ با کلیک روی هر طرح، صدای همان پد پخش می‌شود و بخش مربوطه روشن می‌شود. همین اتفاق با نت‌های MIDI هم رخ می‌دهد.

## پدها

- **KICK** — صدای ورق/دستکاری کاغذ که پایین کوک شده تا بدنه‌ی کیک بدهد.
- **SNARE** — همان خانواده‌ی صدای کاغذ، کوتاه‌تر و روشن‌تر.
- **HAT** — صدای مداد روی کاغذ، کوتاه و بالا کوک‌شده.
- **TEAR** — پاره‌کردن کاغذ.
- **CRUMPLE** — مچاله‌کردن کاغذ.

نت‌های MIDI: `C1 / 36`، `D1 / 38`، `F#1 / 42`، `A#1 / 46`، `C#2 / 49`. اکنون Note-off هم رعایت می‌شود؛ یعنی طول نت تعیین می‌کند صدا چه مدت اجازه‌ی پخش داشته باشد و در انتها با فید بسیار کوتاه قطع می‌شود.

## ساخت در Windows

پیش‌نیازها:

1. Visual Studio 2022 با گزینه‌ی **Desktop development with C++**.
2. CMake نسخه‌ی 3.22 یا جدیدتر.
3. Git برای دریافت JUCE 8.0.8 در زمان configure.

در **x64 Native Tools Command Prompt for VS 2022**:

```bat
cd PaperDrums
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

بعد از build، فایل VST3 معمولاً در این مسیر است:

```text
build\PaperDrums_artefacts\Release\VST3\Paper Drums.vst3
```

اگر DAW پلاگین را پیدا نکرد، آن را در این پوشه کپی کن:

```text
C:\Program Files\Common Files\VST3
```

سپس در DAW اسکن پلاگین‌ها را دوباره انجام بده.

## ساخت خودکار در GitHub

داخل پروژه فایل `.github/workflows/build-windows.yml` قرار دارد. محتویات Extract‌شده‌ی ZIP را در Repository آپلود کن، نه خود ZIP را به‌تنهایی.

بعد از Push:

1. وارد تب **Actions** شو.
2. Workflow به نام **Build Paper Drums VST3 for Windows** را باز کن.
3. وقتی Build سبز شد، از بخش **Artifacts** فایل `PaperDrums-VST3-Windows` را دانلود کن.
4. ZIP را Extract کن و فایل `Paper Drums.vst3` را در این مسیر کپی کن:

```text
C:\Program Files\Common Files\VST3
```

برای قرارگرفتن فایل در بخش Releases هم می‌توانی این کار را انجام بدهی:

```bash
git tag v0.1.0
git push origin v0.1.0
```

## مجوز صداها

فایل‌های WAV از منابع CC0 / public-domain-equivalent انتخاب شده‌اند. لینک منبع و اطلاعات مجوز در `CREDITS.md` ثبت شده است. برای انتشار نسخه‌ی کامپایل‌شده، این فایل را همراه پروژه نگه دار و شرایط مجوز JUCE را هم بررسی کن.
