extern int main();

extern "C" void sched() {
    // Just call main()
    main();
}
