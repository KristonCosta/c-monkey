// Guess will attempt to guess the value provided to it
// and print its attempts.
//
// input: 
//  val - some integer [0,10000]
// output: 
//  void

let guess = fn(val) {
  if (val > 10000) {
    print("Please enter a number in the range [0,10000]")
    return
  }
  if (val < 0) {
    print("Please enter a number in the range [0,10000]")
    return
  }
  let iter = fn(val, hi, lo) {
    let guess = (hi + lo) / 2
    if (guess == val) {
      print(sprint("Got the correct answer! ", val));
    }
    if (val > guess) {
      print(sprint("Guessed ", guess, " but it was too low."))
      iter(val, hi, guess);
    } 
    if (val < guess) {
      print(sprint("Guessed ", guess, " but it was too high."))
      iter(val, guess, lo);
    }
  }
  iter(val, 10001, -1);
}