# 🎨 Tiger Audit Platform - UI/UX Design Guide

**Professional Interface Design for ESP32-V2 Security Toolkit**

---

## 🌈 Design Philosophy

### Core Principles:
1. **Elegance** - Clean, professional appearance
2. **Usability** - Intuitive navigation and controls
3. **Feedback** - Visual confirmation of actions
4. **Performance** - Smooth, responsive interface
5. **Accessibility** - Clear icons and descriptions

---

## 🎨 Visual Language

### Color Palette

#### Primary Colors:
- **Tiger Orange (#FFC000)** - Energy, power, action
- **Electric Blue (#00BFFF)** - Information, active states
- **Lime Green (#00FF00)** - Success, completion
- **Crimson Red (#FF0000)** - Errors, warnings
- **Gold (#FFFF00)** - Alerts, highlights

#### Neutral Colors:
- **Black (#000000)** - Background (dark theme)
- **White (#FFFFFF)** - Text, highlights
- **Dark Gray (#333333)** - Borders, dividers
- **Light Gray (#CCCCCC)** - Disabled states

### Typography

**Fonts:**
- UI: Monospace (terminal compatibility)
- Headers: Bold/Heavy weight
- Body: Regular weight
- Disabled: Dim/Low opacity

**Sizes:**
- Title: 16-20 chars (full width)
- Menu Item: 14-16 chars
- Description: 12-14 chars
- Status: 10-12 chars

---

## 🖼️ Interface Components

### 1. **Boot Screen**

```
╔════════════════════════════════════════╗
║                                        ║
║              🐯 TIGER 🐯               ║
║                                        ║
║    Professional Audit Platform         ║
║       ESP32-S3 Security Toolkit        ║
║                                        ║
║  [████████████░░░░░░░░░] 65% Loading  ║
║                                        ║
║  ✓ Hardware Drivers                    ║
║  ✓ Radio Modules                       ║
║  ✓ Security Systems                    ║
║  ✓ Database Sync                       ║
║  ⟳ Menu System...                     ║
║                                        ║
╚════════════════════════════════════════╝
```

**Features:**
- Tiger ASCII art (large, centered)
- Animated loading bar
- System initialization checklist
- Smooth transitions

### 2. **Status Bar**

```
╔════════════════════════════════════════╗
║ 🔋 [██████░░] 60% 📡 WiFi 🔵 BLE 🕐 14:32║
╚════════════════════════════════════════╝
```

**Elements:**
- Battery indicator with percentage
- WiFi status (connected/disconnected)
- BLE status (active/inactive)
- Current time/temperature

### 3. **Main Menu**

```
╔════════════════════════════════════════╗
║                                        ║
║       🐯 TIGER AUDIT PLATFORM 🐯       ║
║                                        ║
║   Professional Security Toolkit v3.1   ║
║                                        ║
╠════════════════════════════════════════╣
║  SELECT A CATEGORY:                    ║
╠════════════════════════════════════════╣
║ ▶ 📡  RF Tools - Sub-Ghz, NFC, IR     ║
║   🔵  Bluetooth/BLE - Scanning & Attacks║
║   🔌  GPIO & UART - Hardware Testing   ║
║   ⌨️   BadUSB/HID - USB Emulation      ║
║   🦠  Malware Scanner - Threat Detect  ║
║   🔑  iButton - Key Cloning            ║
║   🎮  Games & Tools - Entertainment    ║
║   📁  Archive/Files - Storage Manager  ║
║   ⚙️   Settings - Configuration        ║
║   ❓  Help - Documentation             ║
║   ℹ️   About - Version Info            ║
╚════════════════════════════════════════╝

  ▲▼ Navigate  ● Select  ◄ Back  ? Help
```

**Navigation:**
- ▶ Arrow indicates selected item
- Full descriptions visible
- Icons provide visual hierarchy
- Clear navigation hints

### 4. **Submenu**

```
╔════════════════════════════════════════╗
║  🔵 Bluetooth/BLE                      ║
╠════════════════════════════════════════╣
║ ▶ 🔍 BLE Scanner - Discover devices  ║
║   🔗 Device Connection - Connect     ║
║   🎭 Device Emulation - Emulate BLE  ║
║   👀 BLE Sniffer - Capture packets   ║
║   💥 Spam Attack - Advertisement flood║
╚════════════════════════════════════════╝

  ▲▼ Navigate  ● Select  ◄ Back  ? Info
```

---

## 🎬 Animations & Transitions

### Loading Animation
```
Frames:
  ⠋ Loading...
  ⠙ Loading...
  ⠹ Loading...
  ⠸ Loading...
  ⠼ Loading...
```
- Smooth spinners
- Progress percentage display
- Estimated time remaining

### Menu Transitions
```
Current Menu → Fade → New Menu
Duration: 150ms
```

### Success Animation
```
  ╭─────────────────────────╮
  │  ✅ Action Completed!   │
  ╰─────────────────────────╯
```
- Bright green (#00FF00)
- Checkmark icon
- Brief display (800ms)

### Warning Animation
```
  ╭─────────────────────────╮
  │  ⚠️  Please Confirm      │
  ╰─────────────────────────╯
```
- Bright yellow (#FFFF00)
- Warning triangle
- Longer display (1000ms)

### Error Animation
```
  ╭─────────────────────────╮
  │  ❌ Action Failed!       │
  ╰─────────────────────────╯
```
- Bright red (#FF0000)
- X mark
- Extended display (1500ms)

---

## 📊 Visualizations

### Progress Bar
```
  Loading: [████████░░░░░░░░] 40%
```
- 20-character bar
- Percentage display
- Color-coded (green→yellow→red)

### Battery Graph
```
  🔋 Battery History:
      ████████████ 100%
      ████████░░░░  75%
      ███████░░░░░  60%
      ██████░░░░░░  50%
      █████░░░░░░░  40%
```
- 5-character blocks
- Trend visualization

### Signal Map
```
  📡 Signal Map:
    Channel 1: [███████░░] -45 dBm
    Channel 2: [█████░░░░░] -55 dBm
    Channel 3: [████░░░░░░] -65 dBm
    Channel 4: [██░░░░░░░░] -75 dBm
```
- 10-character bars
- RSSI values
- Visual comparison

### Waveform
```
  📊 Audio Waveform:
    ▁▂▃▄▅▆▇█▆▅▄▃▂▁▂▃▄▅▆▇█
```
- Simple ASCII art
- Data visualization

### Temperature Gauge
```
  🌡️  Temperature: 🔵 COLD 15.2°C
  🌡️  Temperature: 🟢 NORMAL 28.5°C
  🌡️  Temperature: 🟡 WARM 42.3°C
  🌡️  Temperature: 🔴 HOT 58.9°C
```
- Color-coded status
- Temperature value

---

## 🎯 Icon System

### Navigation:
- `▶` - Selected item
- `▲▼` - Navigation arrows
- `◄►` - Back/Forward
- `●` - Select/Confirm

### Status:
- `✅` - Success
- `⚠️` - Warning
- `❌` - Error
- `⟳` - Loading

### Categories:
- `📡` - RF/Radio
- `🔵` - Bluetooth
- `🔌` - GPIO/Hardware
- `⌨️` - Keyboard/USB
- `🦠` - Malware
- `🔑` - Keys/iButton
- `🎮` - Games
- `📁` - Files

### Indicators:
- `🔋` - Battery
- `📶` - Signal
- `🕐` - Time
- `🌡️` - Temperature
- `📊` - Data/Graph
- `🐯` - Tiger logo

---

## 🎨 Theme System

### Dark Theme (Default)
```
Background: #000000 (Black)
Text: #FFFFFF (White)
Borders: #333333 (Dark Gray)
Primary: #FFC000 (Tiger Orange)
Accent: #00BFFF (Electric Blue)
```

### Light Theme (Alternative)
```
Background: #FFFFFF (White)
Text: #000000 (Black)
Borders: #CCCCCC (Light Gray)
Primary: #FF8800 (Dark Orange)
Accent: #0080FF (Dark Blue)
```

### Hacker Theme (Green)
```
Background: #0A0A0A (Dark Green-Black)
Text: #00FF00 (Bright Green)
Borders: #00AA00 (Dark Green)
Primary: #00FF00 (Lime)
Accent: #00FFFF (Cyan)
```

---

## ⌨️ Interaction Pattern

### Navigation Flow:
1. **Display** current menu
2. **Highlight** selected item
3. **Listen** for input
   - ▲ = Move up
   - ▼ = Move down
   - ● = Select
   - ◄ = Back
   - ? = Help
4. **Animate** transition
5. **Display** new menu

### Selection Flow:
```
Menu Item → Confirm → Loading Animation → Result
   ↓           ↓              ↓
Present   User Confirms  Show Status
Options   (Press ●)      (✅/⚠️/❌)
```

---

## 🎯 Best Practices

### DO:
✅ Use consistent spacing (2-4 spaces)
✅ Align text for readability
✅ Provide visual feedback for every action
✅ Use icons to enhance text
✅ Keep menus concise (<12 items)
✅ Show descriptions for complex tools
✅ Provide navigation hints

### DON'T:
❌ Overload screens with text
❌ Use inconsistent formatting
❌ Hide important information
❌ Create confusing navigation
❌ Show irrelevant details
❌ Forget error messages
❌ Disable visual feedback

---

## 📐 Layout Grid

### Standard Menu:
```
Width: 44-50 characters
Height: 12-16 lines
Margin: 2 characters (left/right)
Padding: 1 character (top/bottom)
```

### Status Bar:
```
Width: Full width
Height: 2 lines
Format: ║ content ║
```

### Content Area:
```
Width: 40-42 characters
Height: 6-12 lines
Format: Bordered box (╭─╮ / │ │ / ╰─╯)
```

---

## 🚀 Implementation

### Files:
- `include/ui_enhanced.h` - UI system header
- `src/ui_enhanced.cpp` - UI implementation
- `include/flipper_menu_enhanced.h` - Menu header
- `src/flipper_menu_enhanced.cpp` - Menu implementation

### Integration:
```cpp
#include "ui_enhanced.h"
#include "flipper_menu_enhanced.h"

void setup() {
  UIEnhanced& ui = UIEnhanced::getInstance();
  FlipperMenuEnhanced& menu = FlipperMenuEnhanced::getInstance();
  
  // Display boot screen
  ui.displayAnimatedBootScreen();
  
  // Show main menu
  menu.displayMainMenu();
}
```

---

**Design Version:** 3.1.0  
**Last Updated:** 2026-09-27  
**Status:** ✅ Production Ready

🎨 **Beautiful. Simple. Professional.** 🎨
