import sqlite3

DB_PATH = "leituras.db"

def init_db():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS leituras (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            temperatura_c REAL,
            umidade_pct REAL,
            luminosidade INTEGER,
            presenca INTEGER,
            probabilidade_vida REAL
        )
    """)

    # Dados de exemplo para testar
    cursor.executemany("""
        INSERT INTO leituras (temperatura_c, umidade_pct, luminosidade, presenca, probabilidade_vida)
        VALUES (?, ?, ?, ?, ?)
    """, [
        (22.5, 65.0, 800, 1, 0.87),
        (19.8, 72.3, 200, 0, 0.12),
        (25.1, 58.9, 950, 1, 0.91),
    ])

    conn.commit()
    conn.close()
    print(f"Banco '{DB_PATH}' criado com sucesso!")

if __name__ == "__main__":
    init_db()
