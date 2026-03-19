# Indoor_Health_Monitoring_System - Project Logic & Feature Documentation

## 1. Project Overview
**Name**: Monacos Indoor Health Hub
**Goal**: A comprehensive indoor environmental monitoring system that tracks air quality, comfort, and safety metrics. It provides real-time data, calculates health scores, generates actionable alerts, and features an AI-powered "Health Guardian" assistant.

---

## 2. System Architecture

The system follows a modern client-server architecture:

*   **Backend**: Python (FastAPI)
    *   Handles data ingestion from sensors (Arduino).
    *   Manages business logic (AQI, Health Score, Alerts).
    *   Hosts the AI Agent (LangChain + Google Gemini).
    *   Persists data to specific SQLite database (`monacos.db`).
*   **Frontend**: React + TypeScript (Vite)
    *   Displays real-time dashboard.
    *   Visualizes historical trends.
    *   Provides a chat interface for the AI Guardian.
*   **Hardware (Simulated/Physical)**:
    *   Sends sensor data (Temperature, Humidity, PM2.5, PM10, Noise, Light) via HTTP POST.

---

## 3. Core Features & Logic Breakdown

### A. Data Ingestion & Storage
**Logic Location**: `backend/main.py` -> `/api/ingest`
1.   **Input**: Accepts JSON payload with `device_id`, timestamps, and sensor readings.
2.  **Processing**:
    *   **Live State**: Updates an in-memory dictionary `DEVICE_STATE` for instant access.
    *   **History Buffer**: Appends to an in-memory list `DEVICE_HISTORY` (last 60 readings) for quick graphing.
    *   **Persistence**: Writes every reading to the `sensor_readings` table in SQLite for long-term storage.
3.  **Device Status**: Tracks "Last Seen". Devices silent for >30 seconds are marked **Offline**.

### B. Air Quality Index (AQI) Engine
**Logic Location**: `backend/aqi_engine.py`
*   **Standard used**: US EPA Air Quality Index.
*   **Inputs**: PM2.5 and PM10 concentrations.
*   **Logic**:
    1.  Calculates individual AQI for PM2.5 and PM10 using standard "breakpoints" (logic thresholds).
    2.  Example: PM2.5 between 0-12.0 µg/m³ maps to AQI 0-50 (Good).
    3.  **Final AQI**: The *maximum* of the two individual values (worst-case scenario).
    4.  **Categories**: Good (0-50), Moderate (51-100), Poor (101-150), Unhealthy (151-200), Severe (>200).

### C. Health Score Engine
**Logic Location**: `backend/health_engine.py`
A custom algorithm that quantifies overall room health into a 0-100 score.
*   **Starting Score**: 100 (Perfect).
*   **Penalties**: Points are deducted for deviations from ideal ranges.
    *   **PM2.5**: -20 if > 35, -5 if > 15.
    *   **PM10**: -15 if > 50.
    *   **Noise**: -15 if > 70 dB (Loud).
    *   **Humidity**: -10 if > 65% (Mold risk) or < 30% (Dry).
    *   **Temperature**: -15 if > 28°C (Hot) or < 18°C (Cold).
*   **Result**: 
    *   Score > 80: **Good**
    *   Score > 60: **Moderate**
    *   Score > 40: **Poor**
    *   Score < 40: **Hazardous**

### D. Alert System
**Logic Location**: `backend/alerts_engine.py`
Real-time monitoring for hazardous conditions.
*   **Triggers**:
    *   High PM2.5 (>55) / PM10 (>100).
    *   Excessive Noise (>90 dB).
    *   Extreme Temperature (>30°C or <16°C).
    *   mold-risk Humidity (>75%).
*   **Deduplication Logic**:
    *   Prevents notification spam.
    *   **Cooldown**: Allows only one alert *per type* every 60 seconds.
    *   **Grouping**: Only the latest alert of a specific type (e.g., "High Noise") is kept active.

### E. Recommendation Engine
**Logic Location**: `backend/recommendation_engine.py`
Generates specific actions based on active issues.
*   If **Air Quality Poor**: "Run air purifier on high".
*   If **Noise High**: "Wear protection or isolate source".
*   If **Humidity High**: "Use a dehumidifier".
*   If **Lighting Low (< 50 lux)**: "Open blinds or turn on lights".

### F. AI Health Guardian (Chatbot)
**Logic Location**: `backend/agent_engine.py`
An intelligent agent powered by Google Gemini (LLM) and LangChain.
*   **Capabilities**:
    1.  **Live Awareness**: Can "see" the exact current sensor readings (unlike generic AI's).
    2.  **Context**: The prompt is pre-loaded with a summary of the **Last 7 Days** of data (Avg Temp, Max PM2.5, etc.) so it knows the long-term trends.
    3.  **Tools**:
        *   `predict_air_trend`: Uses real historical data (7 days ) and a **Linear Regression** model (via scikit-learn) to forecast PM2.5 levels for the next 2 hours.
        *   `check_health_standards`: Retrieves the formal Health Score from the data . 
        *   `manage_hardware`: (Simulated) Can "turn on" purifiers or fans if a user asks to .

---

## 4. API Endpoints Summary

### Authentication
*   `POST /auth/signup`: Create account.
*   `POST /auth/login`: Get JWT Access Token.
*   `GET /auth/me`: Get current user profile.

### Data & Devices
*   `POST /api/ingest`: Send sensor data.
*   `GET /api/devices`: List all devices + status (Online/Offline).
*   `GET /api/latest/{id}`: Get raw sensor data.
*   `GET /api/history/{id}`: Get historical chart data (last 7 days).

### Intelligence
*   `GET /api/aqi/{id}`: Get calculated AQI.
*   `GET /api/health-score/{id}`: Get 0-100 score + reasons.
*   `GET /api/alerts/{id}`: Get active alerts.
*   `GET /api/recommendations/{id}`: Get advice list.
*   `POST /api/chat`: Send message to AI Guardian.

---

## 5. Key File References
*   `backend/main.py`: The brain of the API.
*   `backend/monacos.db`: The memory (database).
*   `backend/agent_engine.py`: The AI logic.
*   `backend/*_engine.py`: The specialized logic calculator . 
