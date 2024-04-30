let populate_btn = document.getElementById("populate");
let db_display = document.getElementById("database_display");

function populate_btn_click(e) {
    display_records();
}

async function display_records() {
    const endpoint = "/?=";
    const query = "all";
    const response_obj = await fetch(endpoint + query);
    const records = await response_obj.json();
    const keys = Object.keys(records[0]);
    let keys_para = document.createElement("p");
    keys_para.innerHTML = "";
    db_display.appendChild(keys_para);
    for (const key of keys) {
        keys_para.innerHTML += key + "&emsp;";
    }
    for (record of records) {
        let record_para = document.createElement("p");
        record_para.innerHTML = "";
        for (key in record) {
            record_para.innerHTML += record[key] + "&emsp;" + "&emsp;" + "&emsp;";
        }
        db_display.appendChild(record_para);
    }
}

populate_btn.addEventListener("click", populate_btn_click);