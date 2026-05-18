CREATE TABLE IF NOT EXISTS leituras (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp TEXT,
    temperatura_c REAL,
    umidade_pct REAL,
    luminosidade REAL,
    presenca INTEGER,
    probabilidade_vida REAL
);