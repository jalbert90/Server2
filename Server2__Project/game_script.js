let generate_btn = document.getElementById("generate");
let display = document.getElementById("random_number_display");

function generate_btn_click(e) {
    random_number = Math.round(Math.floor(Math.random() * 10) + 1);
    display.innerHTML = random_number;
}

generate_btn.addEventListener("click", generate_btn_click);