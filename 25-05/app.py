from flask import Flask, jsonify, request
import sqlite3

app = Flask(__name__)
DB_PATH = "leituras.db"


def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row  # retorna linhas como dicionário
    return conn


@app.route("/leituras", methods=["GET"])
def get_leituras():
    """
    Retorna todas as leituras.
    Query params opcionais:
      - limit  (int): máximo de registros (ex: ?limit=10)
      - offset (int): paginação (ex: ?offset=20)
    """
    limit  = request.args.get("limit",  default=None, type=int)
    offset = request.args.get("offset", default=0,    type=int)

    conn = get_db()
    cursor = conn.cursor()

    query = "SELECT * FROM leituras ORDER BY timestamp DESC"
    params = []

    if limit is not None:
        query += " LIMIT ? OFFSET ?"
        params = [limit, offset]

    cursor.execute(query, params)
    rows = cursor.fetchall()
    conn.close()

    return jsonify({
        "total": len(rows),
        "leituras": [dict(row) for row in rows]
    })


@app.route("/leituras", methods=["POST"])
def post_leitura():
    """
    Recebe uma leitura do ESP32 em JSON e salva no banco.
    Campos esperados:
      temperatura_c, umidade_pct, luminosidade, presenca, probabilidade_vida
    """
    dados = request.get_json(silent=True)
    if not dados:
        return jsonify({"erro": "JSON inválido ou ausente"}), 400

    campos = ["temperatura_c", "umidade_pct", "luminosidade", "presenca", "probabilidade_vida"]
    faltando = [c for c in campos if c not in dados]
    if faltando:
        return jsonify({"erro": f"Campos obrigatórios ausentes: {faltando}"}), 400

    conn = get_db()
    cursor = conn.cursor()
    cursor.execute("""
        INSERT INTO leituras (temperatura_c, umidade_pct, luminosidade, presenca, probabilidade_vida)
        VALUES (:temperatura_c, :umidade_pct, :luminosidade, :presenca, :probabilidade_vida)
    """, dados)
    conn.commit()
    novo_id = cursor.lastrowid
    conn.close()

    return jsonify({"mensagem": "Leitura salva com sucesso", "id": novo_id}), 201


@app.route("/leituras/<int:id>", methods=["GET"])
def get_leitura(id):
    """Retorna uma leitura específica pelo ID."""
    conn = get_db()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM leituras WHERE id = ?", (id,))
    row = cursor.fetchone()
    conn.close()

    if row is None:
        return jsonify({"erro": "Leitura não encontrada"}), 404

    return jsonify(dict(row))


if __name__ == "__main__":
    app.run(debug=True, port=5000)
