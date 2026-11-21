
const express = require("express");
const path = require("path");

const app = express();

const PORT = 3999;

// Slozka s webem
app.use(express.static(path.join(__dirname, "public")));

// Aktualni stav mazlicka
let mazlicek = {
    jmeno: "Neznamy",
    druh: "neznamy",
    zdravi: 100,
    hlad: 100,
    unava: 0,
    stesti: 100,
    penize: 0,
    den: 1,
    tah: 0
};

// Prijem dat z C hry
app.get("/api/update", (req, res) => {
    const { jmeno, druh, zdravi, hlad, unava,
            stesti, penize, den, tah } = req.query;

    mazlicek = {
        jmeno: String(jmeno || "Neznamy"),
        druh: String(druh || "neznamy"),
        zdravi: Number(zdravi),
        hlad: Number(hlad),
        unava: Number(unava),
        stesti: Number(stesti),
        penize: Number(penize),
        den: Number(den),
        tah: Number(tah)
    };

    console.log("Prijata data:", mazlicek);
    res.json({ success: true });
});

// Data pro web
app.get("/api/mazlicek", (req, res) => {
    res.json(mazlicek);
});

// Hlavni stranka
app.get("/", (req, res) => {
    res.sendFile(path.join(__dirname, "public", "index.html"));
});

app.listen(PORT, "0.0.0.0", () => {
    console.log(`Server bezi na portu ${PORT}`);
});
