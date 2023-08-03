//
// DESCRIPTION
//   These are  my attempts at project euler questions
//
#include "momo.h"
#include <stdio.h>

// Question 1
//   If we list all the natural numbers below 10 that are multiples 
//   of 3 or 5, we get 3, 5, 6 and 9. The sum of these multiples is 23.
//   Find the sum of all the multiples of 3 or 5 below 1000.
//
// TODO: 
//   Optimize finding multiple of 3
//   Optimize finding multiples of 5
//

static void 
euler_q1() {
  u32_t sum = 0;
  for(u32_t i = 1; i < 1000; ++i) {
    if (i % 3 == 0 || i % 5 == 0) {
      sum += i;
    }
  }
  printf("%d\n", sum);
}

//
// Question 2
//   Each new term in the Fibonacci sequence is generated by adding the previous two terms. 
//   By starting with 1 and 2, the first 10 terms will be:
//
//   1, 2, 3, 5, 8, 13, 21, 34, 55, 89, ...
// 
//   By considering the terms in the Fibonacci sequence whose values do not exceed 
//   four million, find the sum of the even-valued terms.
//

static void 
euler_q2() {
  u32_t x[] = { 0, 1 };
  u32_t sentinel = 0;
  u32_t sum = 0;
  for (;;) {
    u32_t current = x[0] + x[1];
      
    if (current > 4000000 || current < x[sentinel]) {
      break;
    }

    if (!(current & 1)) 
    {
      sum += current;
    }

    x[sentinel] = current;
    sentinel = !sentinel;
  }
  printf("%u\n", sum);
}

// Question 3
//   The prime factors of 13195 are 5, 7, 13 and 29.
//   What is the largest prime factor of the number 600851475143 ?


static void 
euler_q3() {
  u64_t target = 600851475143; 

  printf("%llu\n", target);

  u64_t max = 0;

  while(target != 1) {
    for(u64_t i = 2; i <= target; ++i) {
      if (target % i == 0) {
        //printf("divisor: %lu\n", i);
        max = (i > max) ? i : max; 
        target = target / i;
        break;
      }
    }
  }


  printf("%llu\n", max);


}

//
// Question 4
//
static b32_t 
euler_q4_is_palindrome(u32_t x) {
  // We reverse the number and just compared them
  // instead of comparing each number one by one.
  u32_t rev = 0;
  u32_t tmp = x;


  while(tmp > 0) {
    rev = (rev * 10) + (tmp % 10);
    tmp /= 10;
  }
  return x == rev;
}

static void 
euler_q4() {
  u32_t max = 0;
  for(u32_t i = 999; i >= 100; --i) {
    for(u32_t j = 999; j >= 100; --j) {
      u32_t v = i*j;
      if (euler_q4_is_palindrome(v) && v > max) {
        printf("%d\n", v);
        max = v;
      }
    }
  }

}

//
// Question 5
//
static int
euler_q5_divide_checker(u32_t num) {
  for (u32_t i = 2; i <= 20; ++i ){
    if (num % i != 0) {
      return 0;
    }
  }
  return 1;
}

static void euler_q5() {
  u32_t v = 21;
  while(!euler_q5_divide_checker(v)) { ++v; };
  printf("%d\n", v);

  

}

//
// Question 6
//
static void 
euler_q6() {
  u32_t sum_of_sq = 0;
  u32_t sq_of_sum = 0;

  for (u32_t i = 1; i <= 100; ++i) {
    sum_of_sq += i*i;
    sq_of_sum += i;
  }
  sq_of_sum *= sq_of_sum;

  printf("%u\n", sq_of_sum - sum_of_sq);

}

// what is first 10001st prime number?

static u32_t 
euler_q7_is_prime(u32_t x) {
  for (u32_t i = 2; i < x; ++i) {
    if (x % i == 0) {
      return 0;
    }
  }
  return 1;
}


static void 
euler_q7() {
  u32_t current_number = 2;
  u32_t counter = 0;
  while(1) {
    if (euler_q7_is_prime(current_number)) {
      ++counter;
    }
    if (counter == 10001) 
      break;

    ++current_number;
  }
  printf("%u\n", current_number);
}


// 
// Question 8
//
static void 
euler_q8() {
  char series[] = "7316717653133062491922511967442657474235534919493496983520312774506326239578318016984801869478851843858615607891129494954595017379583319528532088055111254069874715852386305071569329096329522744304355766896648950445244523161731856403098711121722383113622298934233803081353362766142828064444866452387493035890729629049156044077239071381051585930796086670172427121883998797908792274921901699720888093776657273330010533678812202354218097512545405947522435258490771167055601360483958644670632441572215539753697817977846174064955149290862569321978468622482839722413756570560574902614079729686524145351004748216637048440319989000889524345065854122758866688116427171479924442928230863465674813919123162824586178664583591245665294765456828489128831426076900422421902267105562632111110937054421750694165896040807198403850962455444362981230987879927244284909188845801561660979191338754992005240636899125607176060588611646710940507754100225698315520005593572972571636269561882670428252483600823257530420752963450";
   
  u64_t times = 13;
  u64_t max = 0;
  for (u64_t i = 0; i < array_count(series)-times ; ++i) {
    u64_t product = 1;
    for (u64_t j = i; j < i + times; ++j) {
      product *= ascii_to_digit(series[j]);
//      printf("%d ", get(series[j]));
    }
    if (max < product) {
      max = product;
    }

    //printf("= %lu\n", product);
  }

  printf("%llu\n", max);

}


//
// Question 9
//
static u32_t
euler_q9_sq(u32_t x) {
  return x * x;
}
static u32_t 
euler_q9_is_triplet(u32_t a, u32_t b, u32_t c) {

  //printf("Testing: %d, %d, %d\n", sq(a),sq(b),sq(c));
  return (euler_q9_sq(a) + euler_q9_sq(b)) == euler_q9_sq(c);
}

static void 
euler_q9() {
  const u32_t TARGET = 1000;
  u32_t max_a = TARGET / 3 - (TARGET % 3 == 0);  

  for (u32_t a = 1; a <= max_a; ++a) {
    u32_t max_b = (TARGET - a) / 2;
    for (u32_t b = a; b <= max_b; ++b) {
      u32_t c = TARGET - a - b;
      //printf("Testing: %d, %d, %d\n", a,b,c);
      if (euler_q9_is_triplet(a,b,c)) {
        //printf("Triple found: %d, %d, %d\nTheir product is: %d\n", a, b, c, a*b*c); 
        printf("%d\n", a*b*c);
        return;
      }
      //printf("\n");
    }
  }
  printf("No triplet found\n");
  return;
 
  

}

// 
// Question 10
//



static u32_t 
euler_q10_if_prime_return_itself_else_zero(u32_t x) {

  if (x == 1) return 0;
  if (x == 2) return x;

  u32_t until = (u32_t)sqrt(x);
  for (u32_t i = 2; i <= until; ++i) {
    if (x % i == 0) 
      return 0;
  }

  return x;


}

static void 
euler_q10() {
  u64_t sum = 0;
  for (u32_t i = 2; i <= 2000000; ++i) {
    sum += euler_q10_if_prime_return_itself_else_zero(i);
  }
  printf("%llu\n", sum);
}

// 
// Question 11
//



static u32_t euler_q11_grid[] = {
  8, 02, 22, 97, 38, 15, 00, 40, 00, 75, 04, 05, 07, 78, 52, 12, 50, 77, 91, 8,
  49, 49, 99, 40, 17, 81, 18, 57, 60, 87, 17, 40, 98, 43, 69, 48, 04, 56, 62, 00,
  81, 49, 31, 73, 55, 79, 14, 29, 93, 71, 40, 67, 53, 88, 30, 03, 49, 13, 36, 65,
  52, 70, 95, 23, 04, 60, 11, 42, 69, 24, 68, 56, 01, 32, 56, 71, 37, 02, 36, 91,
  22, 31, 16, 71, 51, 67, 63, 89, 41, 92, 36, 54, 22, 40, 40, 28, 66, 33, 13, 80,
  24, 47, 32, 60, 99, 03, 45, 02, 44, 75, 33, 53, 78, 36, 84, 20, 35, 17, 12, 50,
  32, 98, 81, 28, 64, 23, 67, 10, 26, 38, 40, 67, 59, 54, 70, 66, 18, 38, 64, 70,
  67, 26, 20, 68, 02, 62, 12, 20, 95, 63, 94, 39, 63, 8, 40, 91, 66, 49, 94, 21,
  24, 55, 58, 05, 66, 73, 99, 26, 97, 17, 78, 78, 96, 83, 14, 88, 34, 89, 63, 72,
  21, 36, 23, 9, 75, 00, 76, 44, 20, 45, 35, 14, 00, 61, 33, 97, 34, 31, 33, 95,
  78, 17, 53, 28, 22, 75, 31, 67, 15, 94, 03, 80, 04, 62, 16, 14, 9, 53, 56, 92,
  16, 39, 05, 42, 96, 35, 31, 47, 55, 58, 88, 24, 00, 17, 54, 24, 36, 29, 85, 57,
  86, 56, 00, 48, 35, 71, 89, 07, 05, 44, 44, 37, 44, 60, 21, 58, 51, 54, 17, 58,
  19, 80, 81, 68, 05, 94, 47, 69, 28, 73, 92, 13, 86, 52, 17, 77, 04, 89, 55, 40,
  04, 52, 8, 83, 97, 35, 99, 16, 07, 97, 57, 32, 16, 26, 26, 79, 33, 27, 98, 66,
  88, 36, 68, 87, 57, 62, 20, 72, 03, 46, 33, 67, 46, 55, 12, 32, 63, 93, 53, 69,
  04, 42, 16, 73, 38, 25, 39, 11, 24, 94, 72, 18, 8, 46, 29, 32, 40, 62, 76, 36,
  20, 69, 36, 41, 72, 30, 23, 88, 34, 62, 99, 69, 82, 67, 59, 85, 74, 04, 36, 16,
  20, 73, 35, 29, 78, 31, 90, 01, 74, 31, 49, 71, 48, 86, 81, 16, 23, 57, 05, 54,
  01, 70, 54, 71, 83, 51, 54, 69, 16, 92, 33, 48, 61, 43, 52, 01, 89, 19, 67, 48,
};
static u32_t euler_q11_grid_cols = 20; 
static u32_t euler_q11_grid_rows = array_count(euler_q11_grid)/euler_q11_grid_cols;

static u32_t euler_q11_check_hori(u32_t index) {
  return euler_q11_grid[index] * euler_q11_grid[index+1] * euler_q11_grid[index+2] * euler_q11_grid[index+3];
}

static u32_t euler_q11_check_vert(u32_t index) {

  return 
    euler_q11_grid[index+euler_q11_grid_cols*0] * 
    euler_q11_grid[index+euler_q11_grid_cols*1] * 
    euler_q11_grid[index+euler_q11_grid_cols*2] * 
    euler_q11_grid[index+euler_q11_grid_cols*3];
}

static u32_t euler_q11_check_diag_r(u32_t index) {

  return 
    euler_q11_grid[index+euler_q11_grid_cols*0] * 
    euler_q11_grid[index+euler_q11_grid_cols*1+1] * 
    euler_q11_grid[index+euler_q11_grid_cols*2+2] * 
    euler_q11_grid[index+euler_q11_grid_cols*3+3];
}

static u32_t euler_q11_check_diag_l(u32_t index) {
  return 
    euler_q11_grid[index+euler_q11_grid_cols*0] * 
    euler_q11_grid[index+euler_q11_grid_cols*1-1] * 
    euler_q11_grid[index+euler_q11_grid_cols*2-2] * 
    euler_q11_grid[index+euler_q11_grid_cols*3-3];
}
static void euler_q11() {
  u32_t max = 0;
  //printf("h: %d\n", euler_q11_check_hori(0));
  //printf("v: %d\n", euler_q11_check_vert(0));
  //printf("d: %d\n", euler_q11_check_diag(0));
  for (u32_t r = 0; r < euler_q11_grid_rows; ++r) {
    for (u32_t c = 0; c < euler_q11_grid_cols; ++c) {
      u32_t id = r * euler_q11_grid_cols + c;


      if (c < euler_q11_grid_cols-3) {
        max = max_of(max, euler_q11_check_hori(id));
      }
      if (r < euler_q11_grid_rows-3) {
        max = max_of(max, euler_q11_check_vert(id));
      }

      if (c < euler_q11_grid_cols-3 &&
          r < euler_q11_grid_rows-3) {
        max = max_of(max, euler_q11_check_diag_r(id));
      }
      if (c > 3 &&
          r < euler_q11_grid_rows-3) {
        max = max_of(max, euler_q11_check_diag_l(id));
      }
    }
  }

  printf("%d\n", max);


}

// 
// Question 12
//
static void
euler_q12() {


  u32_t index = 1;
  u32_t factor_count = 0;
  u32_t triangle_number;
  do {


    // Get the triangle number
    factor_count = 0;
    triangle_number = 0;
    for (u32_t i = 1; i <= index; ++i) {
      triangle_number += i;
    }
    // find the factors of that number
    for (u32_t factor_a = 1; factor_a <= triangle_number/2; ++factor_a) 
    {
      if (triangle_number % factor_a == 0) {
        u32_t factor_b = triangle_number / factor_a;
        if (factor_a < factor_b) {
          //printf("%d %d ", factor_a, factor_b);
          factor_count += 2;
        }
        else 
          break;
      }
    }

    ++index;

  } while (factor_count <= 500);

  printf("%d\n", triangle_number);

}

// 
// Question 13
//

static void 
euler_q13() {

#if 0
  const char* arr[] = {
    "123",
    "999",
    "999"
  };

#else
  const char* arr[] = {
    "37107287533902102798797998220837590246510135740250",
    "46376937677490009712648124896970078050417018260538",
    "74324986199524741059474233309513058123726617309629",
    "91942213363574161572522430563301811072406154908250",
    "23067588207539346171171980310421047513778063246676",
    "89261670696623633820136378418383684178734361726757",
    "28112879812849979408065481931592621691275889832738",
    "44274228917432520321923589422876796487670272189318",
    "47451445736001306439091167216856844588711603153276",
    "70386486105843025439939619828917593665686757934951",
    "62176457141856560629502157223196586755079324193331",
    "64906352462741904929101432445813822663347944758178",
    "92575867718337217661963751590579239728245598838407",
    "58203565325359399008402633568948830189458628227828",
    "80181199384826282014278194139940567587151170094390",
    "35398664372827112653829987240784473053190104293586",
    "86515506006295864861532075273371959191420517255829",
    "71693888707715466499115593487603532921714970056938",
    "54370070576826684624621495650076471787294438377604",
    "53282654108756828443191190634694037855217779295145",
    "36123272525000296071075082563815656710885258350721",
    "45876576172410976447339110607218265236877223636045",
    "17423706905851860660448207621209813287860733969412",
    "81142660418086830619328460811191061556940512689692",
    "51934325451728388641918047049293215058642563049483",
    "62467221648435076201727918039944693004732956340691",
    "15732444386908125794514089057706229429197107928209",
    "55037687525678773091862540744969844508330393682126",
    "18336384825330154686196124348767681297534375946515",
    "80386287592878490201521685554828717201219257766954",
    "78182833757993103614740356856449095527097864797581",
    "16726320100436897842553539920931837441497806860984",
    "48403098129077791799088218795327364475675590848030",
    "87086987551392711854517078544161852424320693150332",
    "59959406895756536782107074926966537676326235447210",
    "69793950679652694742597709739166693763042633987085",
    "41052684708299085211399427365734116182760315001271",
    "65378607361501080857009149939512557028198746004375",
    "35829035317434717326932123578154982629742552737307",
    "94953759765105305946966067683156574377167401875275",
    "88902802571733229619176668713819931811048770190271",
    "25267680276078003013678680992525463401061632866526",
    "36270218540497705585629946580636237993140746255962",
    "24074486908231174977792365466257246923322810917141",
    "91430288197103288597806669760892938638285025333403",
    "34413065578016127815921815005561868836468420090470",
    "23053081172816430487623791969842487255036638784583",
    "11487696932154902810424020138335124462181441773470",
    "63783299490636259666498587618221225225512486764533",
    "67720186971698544312419572409913959008952310058822",
    "95548255300263520781532296796249481641953868218774",
    "76085327132285723110424803456124867697064507995236",
    "37774242535411291684276865538926205024910326572967",
    "23701913275725675285653248258265463092207058596522",
    "29798860272258331913126375147341994889534765745501",
    "18495701454879288984856827726077713721403798879715",
    "38298203783031473527721580348144513491373226651381",
    "34829543829199918180278916522431027392251122869539",
    "40957953066405232632538044100059654939159879593635",
    "29746152185502371307642255121183693803580388584903",
    "41698116222072977186158236678424689157993532961922",
    "62467957194401269043877107275048102390895523597457",
    "23189706772547915061505504953922979530901129967519",
    "86188088225875314529584099251203829009407770775672",
    "11306739708304724483816533873502340845647058077308",
    "82959174767140363198008187129011875491310547126581",
    "97623331044818386269515456334926366572897563400500",
    "42846280183517070527831839425882145521227251250327",
    "55121603546981200581762165212827652751691296897789",
    "32238195734329339946437501907836945765883352399886",
    "75506164965184775180738168837861091527357929701337",
    "62177842752192623401942399639168044983993173312731",
    "32924185707147349566916674687634660915035914677504",
    "99518671430235219628894890102423325116913619626622",
    "73267460800591547471830798392868535206946944540724",
    "76841822524674417161514036427982273348055556214818",
    "97142617910342598647204516893989422179826088076852",
    "87783646182799346313767754307809363333018982642090",
    "10848802521674670883215120185883543223812876952786",
    "71329612474782464538636993009049310363619763878039",
    "62184073572399794223406235393808339651327408011116",
    "66627891981488087797941876876144230030984490851411",
    "60661826293682836764744779239180335110989069790714",
    "85786944089552990653640447425576083659976645795096",
    "66024396409905389607120198219976047599490197230297",
    "64913982680032973156037120041377903785566085089252",
    "16730939319872750275468906903707539413042652315011",
    "94809377245048795150954100921645863754710598436791",
    "78639167021187492431995700641917969777599028300699",
    "15368713711936614952811305876380278410754449733078",
    "40789923115535562561142322423255033685442488917353",
    "44889911501440648020369068063960672322193204149535",
    "41503128880339536053299340368006977710650566631954",
    "81234880673210146739058568557934581403627822703280",
    "82616570773948327592232845941706525094512325230608",
    "22918802058777319719839450180888072429661980811197",
    "77158542502016545090413245809786882778948721859617",
    "72107838435069186155435662884062257473692284509516",
    "20849603980134001723930671666823555245252804609722",
    "53503534226472524250874054075591789781264330331690"
  };
#endif

  u32_t carry = 0;
  char solution[256] = {};
  u32_t solution_pos = array_count(solution); 
  s32_t pos = 50 - 1;
  while(pos >= 0) {
    u32_t sum_of_ones = 0;
    for (u32_t i = 0; i < array_count(arr); ++i) {
      sum_of_ones += ascii_to_digit(arr[i][pos]);
    }
    carry += sum_of_ones;

    u32_t ones = carry % 10;
    solution[--solution_pos] = (char)digit_to_ascii(ones);

    //printf("%d: %d: %d: %s\n", pos, carry, sum_of_ones, solution + solution_pos);
    carry /= 10; 
    pos--;
  }

  while (carry > 0) {
    //printf("carry: %d\n", carry);
    solution[--solution_pos] = digit_to_ascii(carry%10);
    carry/=10;
  }

  solution[solution_pos + 10] = 0;

  printf("%s\n", solution + solution_pos);

}

//
// Question 14
//
static void
euler_q14() {
  u32_t start_n = 1;

  u32_t highest_chain = 1;
  u32_t highest_n = 1;

  while(start_n < 1000000){
    u32_t chain = 1;
    u32_t n = start_n;
    //printf("n: %u\n", n);
    while(n != 1){
      if (n % 2 == 0) {
        n /= 2;
      }
      else {
        n = 3*n + 1;
      }
      ++chain;
    }
    //printf("chain: %u\n\n", chain);
    if (chain > highest_chain) {
      highest_chain = chain;
      highest_n = start_n;
    }
    ++start_n;
  }
  printf("n: %u has the highest chain at %u\n", highest_n, highest_chain);

}

// 
// Question 15
//
// NOTE(momo): 
//  Feels like a combinatorics question.
//
//  Let each direction be represented by a letter:
//    R = Right
//    D = Down
//
//  The width represents how many R's there are  
//  while the height represents how D's there are.
//
//  All we have to do is then calculate how many
//  permutations of D's and R's we can get.
//
//  For example, 2x2 means 2 D's and 2 Rs.
//  Permutations are:
//    RRDD
//    RDRD
//    RDDR
//    DDRR
//    DRDR
//    DRRD
//
//  = 6 permutations.
//  
//  The mathematical formula for this is the "Permutations with Reptition":
//    n!/(r1! * r2! * ... * rk!)
//  where 
//    n = Total number of characters
//    r1 = Occurances of first letter
//    r2 = Occurances of second letter
//    .
//    .
//    rk = Occurances of kth letter
//    
//

static void
euler_q15() {
  f64_t n = f64_factorial(40);
  f64_t d = f64_factorial(20) * u64_factorial(20);
  printf("Answer: %0.f\n", n/d);
}

//
// Question 16
//
// 2^1000 gets pretty huge that we can't store it
// in a u64. Floating points have also proven to be
// grossly inaccurate. The next easiest way to implement
// this would be to have a vector of digits and perform
// operations on it
//
// NOT SOLVED
//

struct euler_16_big_num {
  // TODO
};

static void
euler_q16() {
  f64_t n = f64_pow(2, 1000);

  u32_t sum = 0;
  while(!f64_is_close(n, 0.0)) {
    f64_t m = f64_mod(n, 10);
    sum += (u32_t)m; 
    n = trunc(n/10.0);
  }

  printf("Answer: %u\n", sum);
}

int main() {
  //euler_q1();
  //euler_q2();
  //euler_q3();
  //euler_q4();
  //euler_q5();
  //euler_q6();
  //euler_q7();
  //euler_q8();
  //euler_q9();
  //euler_q10();
  //euler_q11();
  //euler_q12();
  //euler_q13();
  //euler_q14();
  //euler_q15();
  euler_q16();
}


