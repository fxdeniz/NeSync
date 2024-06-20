addEventListener("DOMContentLoaded", (event) => {
    const buttonFileExplorer = document.getElementById("button-file-explorer");

    buttonFileExplorer.addEventListener('click', async clickEvent => {
        window.router.routeToFileExplorer();
    });
});