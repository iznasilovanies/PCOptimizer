# PC Optimizer Premium - Инструкция по сборке

## Шаг 1: Установка Visual Studio 2022

Скачайте и установите Visual Studio 2022 Community с компонентами:
- Desktop development with C++
- Windows 10/11 SDK

## Шаг 2: Установка vcpkg

```powershell
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

## Шаг 3: Установка зависимостей

```powershell
cd C:\Users\Administrator\Desktop\PCOptimizer
C:\vcpkg\vcpkg install --triplet=x64-windows
```

Это автоматически установит все зависимости из `vcpkg.json`:
- ImGui (с docking и DirectX 11)
- nlohmann-json (для JSON профилей)
- spdlog (для логирования)

## Шаг 4: Генерация проекта CMake

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

## Шаг 5: Компиляция

### Через командную строку:

```powershell
cmake --build . --config Release
```

### Через Visual Studio:

1. Открыть `build\PCOptimizer.sln`
2. Выбрать конфигурацию Release / x64
3. Build → Build Solution (Ctrl+Shift+B)

## Шаг 6: Запуск

```powershell
.\Release\PCOptimizer.exe
```

---

## Troubleshooting

### Ошибка: "Cannot find ImGui"

Убедитесь, что vcpkg установлен корректно:

```powershell
C:\vcpkg\vcpkg list imgui
```

Должно показать `imgui:x64-windows`.

### Ошибка: "LNK2019 unresolved external symbol"

Очистите и пересоберите проект:

```powershell
cd build
cmake --build . --config Release --clean-first
```

### Ошибка: "VCPKG_ROOT not defined"

Установите переменную окружения:

```powershell
[System.Environment]::SetEnvironmentVariable('VCPKG_ROOT', 'C:\vcpkg', 'User')
```

---

## Следующие шаги

После успешной компиляции вы увидите демо-окно с кастомными UI компонентами:
- Кнопки с ripple эффектом
- Слайдеры с анимацией
- Toggle switches
- Карточки с hover elevation

Далее будут добавлены:
- Sidebar navigation
- Dashboard виджеты
- Performance оптимизаторы
- AI analyzer
