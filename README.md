# Monacos - Indoor Health Monitoring System

![Status](https://img.shields.io/badge/Status-Active-success)
![Stack](https://img.shields.io/badge/Stack-React%20%7C%20FastAPI%20%7C%20Gemini-blue)

**Monacos** is a comprehensive indoor environmental monitoring system designed to track, analyze, and improve your living space's health. It combines real-time sensor data with advanced AI insights to provide actionable recommendations.

## 🚀 Key Features

### 📊 Real-Time Dashboard
- **Live Monitoring**: Visualize critical metrics including Temperature, Humidity, PM2.5, PM10, Noise, and Light levels.
- **Health Score**: An aggregated 0-100 health score algorithm based on WHO and ASHRAE standards.
- **Smart Alerts**: Instant notifications for hazardous conditions (e.g., High CO2, Poor Air Quality) with debounce logic.

### 🤖 AI Health Guardian (Chatbot)
- **Powered by Google Gemini**: Uses the latest Flash model for fast, accurate responses.
- **Context-Aware**: The bot has access to **7 days of historical data**, allowing it to analyze trends and provide personalized advice.
- **Predictive Capabilities**: Ask "What will the temperature be in 2 hours?" to get AI-driven forecasts using linear regression.
- **Hardware Control**: The AI can suggest or even simulate controlling devices like purifiers and fans.
- **Rate Limit Handling**: Gracefully handles API quotas with helpful user feedback.

### 🔐 User Authentication & Profiles
- **Secure Access**: JWT-based Login and Signup functionality to protect your data.
- **Profile Management**: Customize your experience with avatar selection (Dicebear integration) and personal details.
- **Member Dashboard**: View "Member Since" status and manage account settings.

### 📡 Bluetooth Connectivity
- **Web Bluetooth Integration**: Connect directly to your ESP32 sensor hub from the browser.
- **Real-Time Sync**: Stream sensor data straight from the hardware to the dashboard without intermediate servers.
- **Easy Setup**: Click "Connect Bluetooth" in the Devices header to pair.

### 📈 Advanced Analytics
- **Historical Trends**: View data over the last 24 hours or 7 days with interactive charts.
- **Device Management**: Add, name, and manage multiple sensor units (e.g., "Living Room", "Bedroom") maintained in a persistent database.

## 🛠️ Technology Stack

### Frontend
- **Framework**: React (Vite)
- **Styling**: TailwindCSS, Shadcn UI
- **Animations**: Framer Motion
- **Visualization**: Recharts
- **State Management**: Context API

### Backend
- **API**: FastAPI (Python)
- **Database**: SQLite (Async/Sync hybrid access)
- **AI Engine**: LangChain + Google Generative AI (Gemini 2 Flash)
- **Authentication**: OAuth2 with JWT (JSON Web Tokens)

## 📦 Installation & Setup

### Prerequisites
- Python 3.9+
- Node.js 16+
- Google Gemini API Key

### 1. Backend Setup

```bash
cd backend
# Install dependencies
pip install -r requirements.txt

# Configure Environment
# Create a .env file in /backend with:
GOOGLE_API_KEY=your_api_key_here

# Start the Server
uvicorn main:app --host 0.0.0.0 --port 8000
```

### 2. Frontend Setup

```bash
cd Indoor_Health_Monitoring_System
# Install dependencies
npm install

# Start Development Server
npm run dev
```

## 📱 Usage Guide

1. **Dashboard Access**: Open `http://localhost:8080`.
2. **Sign Up / Login**: Create an account to access the full dashboard and personalized settings.
3. **Connect Device**:
    - Go to the **Devices** page.
    - Click **Connect Bluetooth**.
    - Select your **Monacos_Hub** device.
4. **Ask the AI**:
    - Click the chat icon in the bottom right.
    - Try asking: *"How has the air quality been this week?"* or *"Predict the temperature trend."*
5. **Manage Profile**:
    - Click your avatar in the top right or go to **Settings** to update your profile or sign out.

---

