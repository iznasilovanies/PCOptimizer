# PC Optimizer Premium

Премиум-инструмент для оптимизации Windows с современным ImGui интерфейсом и полным user-mode функционалом.

## 🎯 Возможности

### ✅ Реализованные Компоненты

#### 8 Полнофункциональных Оптимизаторов

1. **Timer Optimizer** - NT API timer resolution (0.5-15.6ms)
2. **Power Optimizer** - Power plans, core parking, throttling
3. **Thread Optimizer** - Process/thread affinity & priority
4. **Interrupt Optimizer** - IRQ routing через реестр
5. **Memory Optimizer** - Memory priority, standby list, working set
6. **Quantum Tweaker** - Win32PrioritySeparation настройки
7. **Network Optimizer** - TCP/QoS/RSS оптимизации
8. **Profile Manager** - 4 готовых профиля с JSON сохранением

#### AI System Analyzer

- Автообнаружение gaming/streaming процессов
- Анализ загрузки CPU/RAM
- Приоритизированные рекомендации
- Auto-apply поддержка

#### Monitoring Engine

- Real-time CPU (per-core), RAM, Disk мониторинг
- Отдельный поток с настраиваемой частотой
- PDH API интеграция

---

## 📂 Структура Проекта

```
PCOptimizer/
├── src/
│   ├── ai/
│   │   └── ai_analyzer.h/cpp          # AI анализатор системы
│   ├── monitoring/
│   │   └── monitoring_engine.h/cpp    # Движок мониторинга
│   ├── optimizers/                    # 8 оптимизаторов
│   │   ├── thread_optimizer.h/cpp
│   │   ├── timer_optimizer.h/cpp
│   │   ├── power_optimizer.h/cpp
│   │   ├── interrupt_optimizer.h/cpp
│   │   ├── memory_optimizer.h/cpp
│   │   ├── quantum_tweaker.h/cpp
│   │   ├── network_optimizer.h/cpp
│   │   └── profile_manager.h/cpp
│   ├── ui/ (legacy components)
│   └── main_new.cpp                   # Главный UI
├── build/Release/
│   └── PCOptimizer.exe                # 543 KB binary
└── CMakeLists.txt
```

---

## Сборка

### Требования

- Visual Studio 2022
- CMake 3.20+
- vcpkg (для зависимостей)

### Установка зависимостей

```bash
vcpkg install imgui[docking-experimental,dx11-binding,win32-binding]
vcpkg install nlohmann-json
vcpkg install spdlog
```

### Компиляция

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

## Использование

1. Запустить `PCOptimizer.exe` с правами администратора
2. Выбрать профиль оптимизации (Gaming/Streaming/Workstation)
3. Применить рекомендации AI или настроить вручную
4. Мониторить результаты на Dashboard

---

## ✅ Реализованные Профили

### 1. Gaming Profile
- Timer: 0.5ms (минимальный input lag)
- Core Parking: 100% disabled
- Network: TcpAckFrequency=1, TCPNoDelay, disable Nagling
- Quantum: Foreground optimized, variable, short

### 2. Streaming Profile  
- Timer: 1.0ms (баланс)
- Core Parking: 75-100% active
- Network: Optimized buffers, RSS enabled
- Quantum: Long, foreground boost

### 3. Workstation Profile
- Timer: 1.0ms
- Core Parking: 50% balanced
- Network: TCP optimizations
- Quantum: Long, fixed

### 4. Balanced Profile
- Все настройки по умолчанию
- Reset network/power settings

## 🚀 Технологии

- C++20, MSVC 2022
- ImGui 1.91.9 (vanilla vcpkg version)
- DirectX 11
- nlohmann-json, spdlog
- Windows NT API (NtSetTimerResolution, etc.)
- PDH API, PowrProf, SetupAPI

---

## ⚠️ Требования

**Администраторские права обязательны** для большинства оптимизаций:

- ✅ Без админ-прав: CPU/RAM monitoring, перечисление процессов
- ❌ Требуют админ-права: Timer resolution, Power plans, Core parking, Registry tweaks

**Безопасность:**
- Все изменения обратимы
- Registry changes сохраняются после перезагрузки
- Timer resolution сбрасывается при закрытии
- Используйте Balanced профиль для reset

## 📋 Статус Реализации

### ✅ Полностью Готово
- [x] 8 Optimizers (Thread, Timer, Power, Interrupt, Memory, Quantum, Network, ProfileManager)
- [x] AI Analyzer с auto-detection
- [x] Monitoring Engine (CPU/RAM/Disk)
- [x] 4 Profiles с реальными настройками
- [x] Dashboard UI с Quick Actions
- [x] Performance Tab с live controls
- [x] Settings Tab
- [x] CMake build system

### ⚠️ Частично
- [⚠️] GPU Monitoring (placeholder, нужна NVML/ADL)
- [⚠️] Network Monitoring (placeholder)

### ❌ Не Реализовано
- [ ] Rollback System (backup реестра)
- [ ] Settings Persistence (JSON save)
- [ ] Advanced UI (drag-drop widgets, graphs)

---

**Версия:** 1.0.0  
**Дата:** 2025-11-13  
**Build:** Release x64 (543 KB)  
**Статус:** ✅ Компилируется, базовый функционал работает
