from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from datetime import datetime
import sqlite3

app = FastAPI()

class LeituraSensor(BaseModel):
    temperatura_c: float
    umidade_pct: float
    luminosidade: float
    presenca: int
    probabilidade_vida: float

def init_db():
    conn = sqlite3.connect("database.db")
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS leituras (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            temperatura_c REAL,
            umidade_pct REAL,
            luminosidade REAL,
            presenca INTEGER,
            probabilidade_vida REAL
        )
    """)
    conn.commit()
    conn.close()

init_db()

@app.post("/leituras")
def salvar_leitura(dados: LeituraSensor):
    try:
        conn = sqlite3.connect("database.db")
        cursor = conn.cursor()
        timestamp_atual = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
        
        cursor.execute("""
            INSERT INTO leituras (timestamp, temperatura_c, umidade_pct, luminosidade, presenca, probabilidade_vida)
            VALUES (?, ?, ?, ?, ?, ?)
        """, (timestamp_atual, dados.temperatura_c, dados.umidade_pct, dados.luminosidade, dados.presenca, dados.probabilidade_vida))
        
        conn.commit()
        conn.close()
        return {"status": "sucesso", "mensagem": "Dados gravados."}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/leituras")
def buscar_leituras():
    conn = sqlite3.connect("database.db")
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM leituras ORDER BY id DESC LIMIT 100")
    linhas = cursor.fetchall()
    conn.close()
    
    resultado = []
    for linha in linhas:
        resultado.append({
            "id": linha[0],
            "timestamp": linha[1],
            "temperatura_c": linha[2],
            "umidade_pct": linha[3],
            "luminosidade": linha[4],
            "presenca": linha[5],
            "probabilidade_vida": linha[6]
        })
    return resultado