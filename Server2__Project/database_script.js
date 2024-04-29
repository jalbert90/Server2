let populate_btn = document.getElementById("populate");
let db_display = document.getElementById("database_display");

function populate_btn_click(e) {
    display_records();
}

async function display_records() {
    const endpoint = "/?query=";
    const query = "all";
    const response_obj = await fetch(endpoint + query);
    const records = await response_obj.json();
    db_display.innerHTML = JSON.stringify(records[0]);
}

populate_btn.addEventListener("click", populate_btn_click);