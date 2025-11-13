# PC Optimizer Premium - Quick Start (без vcpkg)

К сожалению, для полной компиляции требуется установка нескольких инструментов:

## Необходимые компоненты

1. **Visual Studio 2022** с Desktop C++ workload
2. **CMake** 3.20+ ([скачать](https://cmake.org/download/))
3. **vcpkg** для зависимостей
4. **Git** (уже установлен)

## Автоматическая установка

Выполните эти команды в PowerShell (от администратора):

```powershell
# 1. Установка vcpkg
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# 2. Установка зависимостей для PC Optimizer
cd C:\Users\Administrator\Desktop\PCOptimizer
C:\vcpkg\vcpkg install imgui[docking-experimental,dx11-binding,win32-binding]:x64-windows
C:\vcpkg\vcpkg install nlohmann-json:x64-windows
C:\vcpkg\vcpkg install spdlog:x64-windows

# 3. Компиляция проекта
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release

# 4. Запуск
.\Release\PCOptimizer.exe
```

## Альтернатива: Pre-built Demo

Я могу создать упрощённую версию с inline ImGui (без vcpkg), но она будет иметь ограничения.

**Хотите:**
1. Подождать установку инструментов и полную компиляцию (~15-20 минут)
2. Создать упрощённую standalone версию (5 минут, но без полного функционала)
3. Получить готовый .exe (если я создам pre-built binary)

---

## Текущий статус

- ✅ Исходный код готов (18 файлов, ~2500 строк)
- ✅ Структура проекта готова
- ⏳ Ожидание установки build tools
- ⏳ Компиляция

**Рекомендация:** Вариант 1 (полная компиляция) для production-ready demo.
