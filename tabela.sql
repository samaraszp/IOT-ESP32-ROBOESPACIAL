CREATE TABLE leituras (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    temperatura_c REAL,             
    umidade_pct REAL,               
    luminosidade INTEGER,           
    presenca INTEGER,               
    probabilidade_vida REAL
);