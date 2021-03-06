<?
#
# RainbowTable for PHP seeds class
# Version: 0.01 09/09/12
#
# Only for education. Author @d0znpp
# PHP mt_srand seed bruteforce code 
# Usefull to recover full string which generated by mt_rand calls threw first 5 bytes
# Details available at http://www.slideshare.net/d0znpp/dcg7812-cryptographyinwebapps-14052863
# ONsec, 2012 http://lab.onsec.ru @ONsec_lab
class RainbowTable
{
private $Filename="";
private $Deep=1;
private $File=null;
private $RowsCount=0;
private $FileSize=0;
public function RainbowTable($file,$deep){
        $this->Filename=$file;
        $this->Deep=$deep+0;
        if(!is_file($file) || !file_exists($file)) throw new Exception("Could not open rainbowtable file: $file");
        $this->FileSize = filesize($file);
        $this->RowsCount = $this->FileSize/(4*($this->Deep+1));
        if('integer'!==gettype($this->RowsCount)) throw new Exception("Rainbowtable file is corrupted!");
        $this->File=@fopen($file,'rb');
        if(!$this->File){
                throw new Exception("Could not get access to rainbowtable file: $file");
        }
}
public function getRowsCount() { return $this->RowsCount; }
public function readRow($rownum){
        fseek($this->File,$rownum*4*($this->Deep+1));
        $row = fread($this->File,4*($this->Deep+1));
        $res=array();
        for($i=0;$i<$this->Deep+1;$i++){
                $res[$i]=current(unpack('N',substr($row,$i*4,4)));
        }
        return $res;
}
public function selfCheck(){
        $res=$this->readRow(mt_rand(0,$this->RowsCount));
        mt_srand($res[0]);
        for($i=1;$i<count($res);$i++){
                $check_val = mt_rand();
                if($check_val!=$res[$i])
                        return false;
        }
        return true;
}
function __destruct(){
        fclose($this->File);
}
}
?>
